#!/usr/bin/env python3
"""Load PRN chips through AXI-Lite and capture finite I/Q samples from Linux.

Requires the matching ZCU102 bitstream and /dev/mem access. Not tested on board.
"""

import argparse
import csv
import math
import os
import struct
import time

from capture_tone import Window, signed16

PRN_BASE = 0x87060000
BRAM_BASE = 0x87070000
GPIO_BASE = 0x87080000


def float32_bits(value):
    return struct.unpack("<I", struct.pack("<f", value))[0]


def load_chips(path):
    chips = []
    with open(path, newline="", encoding="utf-8") as source:
        for line, row in enumerate(csv.reader(source), 1):
            if not row or row[0].strip().startswith("#"):
                continue
            if len(row) != 2:
                raise ValueError(f"{path}:{line}: expected two columns: i,q")
            try:
                i, q = (int(value.strip(), 0) for value in row)
            except ValueError as exc:
                raise ValueError(f"{path}:{line}: expected signed integer chips") from exc
            if not -128 <= i <= 127 or not -128 <= q <= 127:
                raise ValueError(f"{path}:{line}: chips must be signed 8-bit")
            chips.append((i, q))
    if not 1 <= len(chips) <= 4096:
        raise ValueError("chip count must be in 1..4096")
    return chips


def packed_chip_words(chips):
    """Pack two I/Q int8 chips into each little-endian AXI-Lite word."""
    for index in range(0, len(chips), 2):
        i0, q0 = chips[index]
        word = (i0 & 0xFF) | ((q0 & 0xFF) << 8)
        if index + 1 < len(chips):
            i1, q1 = chips[index + 1]
            word |= ((i1 & 0xFF) << 16) | ((q1 & 0xFF) << 24)
        yield word


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chips-csv", required=True,
                        help="headerless CSV: one signed 8-bit I,Q chip per line")
    parser.add_argument("--samples", type=int, default=1024)
    parser.add_argument("--sample-step", type=float, required=True,
                        help="chips advanced per output sample")
    parser.add_argument("--prn-phase-init", type=float, default=0.0,
                        help="initial chip index; effective only after PL reset")
    parser.add_argument("--doppler-hz", type=float, default=0.0)
    parser.add_argument("--carrier-phase-init", type=float, default=0.0,
                        help="initial radians; effective only after PL reset")
    parser.add_argument("--fs-hz", type=float, required=True,
                        help="numeric sample rate passed to HLS (Hz)")
    parser.add_argument("--timeout", type=float, default=5.0)
    parser.add_argument("--output", default="prn_capture.csv")
    return parser.parse_args()


def main():
    args = parse_args()
    chips = load_chips(args.chips_csv)
    if not 1 <= args.samples <= 1024:
        raise SystemExit("--samples must be in 1..1024")
    numeric = (args.sample_step, args.prn_phase_init, args.doppler_hz,
               args.carrier_phase_init, args.fs_hz, args.timeout)
    if not all(math.isfinite(value) for value in numeric):
        raise SystemExit("all floating-point arguments must be finite")
    if not 0 < args.sample_step <= len(chips):
        raise SystemExit("--sample-step must be in (0, chip count]")
    if not 0 <= args.prn_phase_init < len(chips):
        raise SystemExit("--prn-phase-init must be in [0, chip count)")
    if not 0 <= args.carrier_phase_init < 2 * math.pi:
        raise SystemExit("--carrier-phase-init must be in [0, 2*pi)")
    if args.fs_hz <= 0 or args.timeout <= 0 or abs(args.doppler_hz) > args.fs_hz:
        raise SystemExit("require positive fs/timeout and |doppler| <= fs")
    try:
        params = [float32_bits(value) for value in
                  (args.sample_step, args.prn_phase_init, args.doppler_hz,
                   args.carrier_phase_init, args.fs_hz)]
    except (OverflowError, struct.error) as exc:
        raise SystemExit("a floating-point parameter is outside float32 range") from exc

    fd = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
    # The HLS chip memory occupies offsets 0x2000..0x3fff of the control window.
    prn = bram = gpio = None
    try:
        prn = Window(fd, PRN_BASE, 0x4000)
        bram = Window(fd, BRAM_BASE)
        gpio = Window(fd, GPIO_BASE)
        gpio.write(0x04, 0)
        gpio.write(0x0C, 0xFFFFFFFF)
        gpio.write(0x00, 0)
        prn.write(0x00, 0)  # auto_restart=0
        if not prn.read(0x00) & 0x4:
            raise RuntimeError("PRN generator is not idle; reset/reload the FPGA")

        # Packed memory: chip 2n occupies bits 15:0, chip 2n+1 bits 31:16;
        # within each chip I occupies bits 7:0 and Q bits 15:8.
        for word_index, word in enumerate(packed_chip_words(chips)):
            prn.write(0x2000 + 4 * word_index, word)

        prn.write(0x10, args.samples)
        prn.write(0x18, len(chips))
        for offset, value in zip((0x20, 0x28, 0x30, 0x38, 0x40), params):
            prn.write(offset, value)

        control = args.samples << 16
        gpio.write(0x00, control)
        gpio.write(0x00, control | 1)
        gpio.write(0x00, control)
        state = gpio.read(0x08)
        if state & 0x6000 or not state & 0x1000:
            raise RuntimeError(f"capture did not arm: status=0x{state:08x}")

        prn.write(0x00, 1)
        deadline = time.monotonic() + args.timeout
        while True:
            state = gpio.read(0x08)
            if state & 0x6000:
                raise RuntimeError(f"capture error: status=0x{state:08x}")
            if state & 0x0800:
                break
            if time.monotonic() >= deadline:
                raise TimeoutError(f"capture timed out: status=0x{state:08x}")
            time.sleep(0.001)
        if (state & 0x7FF) != args.samples:
            raise RuntimeError(f"count mismatch: status=0x{state:08x}")
        words = [bram.read(4 * index) for index in range(args.samples)]
        hls_status = prn.read(0x00)
    finally:
        for window in (prn, bram, gpio):
            if window is not None:
                window.close()
        os.close(fd)

    with open(args.output, "w", newline="", encoding="utf-8") as output:
        writer = csv.writer(output)
        writer.writerow(("sample", "word_hex", "i_raw", "q_raw", "i_value", "q_value"))
        for index, word in enumerate(words):
            i, q = signed16(word), signed16(word >> 16)
            writer.writerow((index, f"0x{word:08x}", i, q,
                             i / 16384.0, q / 16384.0))
    print(f"Captured {len(words)} PRN samples to {args.output}; "
          f"capture status=0x{state:08x}, HLS status=0x{hls_status:08x}")


if __name__ == "__main__":
    main()
