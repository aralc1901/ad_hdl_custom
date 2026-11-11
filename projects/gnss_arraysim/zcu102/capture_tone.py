#!/usr/bin/env python3
"""Configure one finite tone capture through /dev/mem and save signed I/Q CSV.

Requires a loaded bitstream/XSA-derived Linux setup that exposes the PL AXI
addresses. This program is not an FPGA test until run on the target board.
"""

import argparse
import csv
import math
import mmap
import os
import struct
import time

TONE_BASE = 0x87000000
BRAM_BASE = 0x87010000
GPIO_BASE = 0x87020000
PAGE_SIZE = 4096


class Window:
    def __init__(self, fd, base, size=PAGE_SIZE):
        page_base = base & ~(PAGE_SIZE - 1)
        self.bias = base - page_base
        self.mapping = mmap.mmap(fd, size + self.bias, offset=page_base)

    def read(self, offset):
        return struct.unpack_from("<I", self.mapping, self.bias + offset)[0]

    def write(self, offset, value):
        struct.pack_into("<I", self.mapping, self.bias + offset, value & 0xFFFFFFFF)

    def close(self):
        self.mapping.close()


def signed16(value):
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def expected_raw(amplitude_raw, phase):
    amplitude = amplitude_raw / 16384.0
    angle = 2.0 * math.pi * phase / 4294967296.0
    i = math.floor(amplitude * math.cos(angle) * 16384.0)
    q = math.floor(amplitude * math.sin(angle) * 16384.0)
    return signed16(i), signed16(q)


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--samples", type=int, default=1024)
    parser.add_argument("--amplitude-raw", type=lambda s: int(s, 0), required=True,
                        help="signed Q2.14 integer, e.g. 4096 for 0.25")
    parser.add_argument("--phase-step", type=lambda s: int(s, 0), required=True,
                        help="unsigned 32-bit NCO increment")
    parser.add_argument("--phase-start", type=lambda s: int(s, 0),
                        help="phase accumulator before this run; supply 0 only after reset")
    parser.add_argument("--sample-rate-hz", type=float,
                        help="actual configured sample rate, for frequency reporting")
    parser.add_argument("--timeout", type=float, default=5.0)
    parser.add_argument("--output", default="tone_capture.csv")
    parser.add_argument("--tolerance-lsb", type=int, default=3)
    return parser.parse_args()


def main():
    args = parse_args()
    if not 1 <= args.samples <= 1024:
        raise SystemExit("--samples must be in 1..1024")
    if not -32768 <= args.amplitude_raw <= 32767:
        raise SystemExit("--amplitude-raw must be a signed 16-bit integer")
    if not 0 <= args.phase_step <= 0xFFFFFFFF:
        raise SystemExit("--phase-step must be an unsigned 32-bit integer")
    if args.phase_start is not None and not 0 <= args.phase_start <= 0xFFFFFFFF:
        raise SystemExit("--phase-start must be an unsigned 32-bit integer")
    if args.timeout <= 0 or args.tolerance_lsb < 0:
        raise SystemExit("timeout must be positive and tolerance must be nonnegative")
    if args.sample_rate_hz is not None and args.sample_rate_hz <= 0:
        raise SystemExit("--sample-rate-hz must be positive")

    fd = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
    tone, bram, gpio = (Window(fd, address) for address in
                        (TONE_BASE, BRAM_BASE, GPIO_BASE))
    try:
        # AXI GPIO channel 1 is output; channel 2 is input.
        gpio.write(0x04, 0x00000000)
        gpio.write(0x0C, 0xFFFFFFFF)
        gpio.write(0x00, 0)

        # HLS control: clear auto-restart and require an idle generator.
        tone.write(0x00, 0)
        if not tone.read(0x00) & 0x4:
            raise RuntimeError("tone generator is not idle; reset/reload the FPGA")
        tone.write(0x10, args.samples)
        tone.write(0x18, args.amplitude_raw & 0xFFFF)
        tone.write(0x20, args.phase_step)

        # Arm the sink first. Each AXI write is observed in the 100 MHz domain.
        control = args.samples << 16
        gpio.write(0x00, control)
        gpio.write(0x00, control | 1)
        gpio.write(0x00, control)
        state = gpio.read(0x08)
        if state & 0x6000 or not state & 0x1000:
            raise RuntimeError(f"capture did not arm: status=0x{state:08x}")

        tone.write(0x00, 1)  # ap_start, auto_restart=0
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
        hls_status = tone.read(0x00)  # ap_done is clear-on-read
    finally:
        tone.close()
        bram.close()
        gpio.close()
        os.close(fd)

    rows = []
    max_error = 0
    for index, word in enumerate(words):
        i_raw, q_raw = signed16(word), signed16(word >> 16)
        row = {"sample": index, "word_hex": f"0x{word:08x}",
               "i_raw": i_raw, "q_raw": q_raw,
               "i_value": i_raw / 16384.0, "q_value": q_raw / 16384.0}
        if args.phase_start is not None:
            phase = (args.phase_start + (index + 1) * args.phase_step) & 0xFFFFFFFF
            i_ref, q_ref = expected_raw(args.amplitude_raw, phase)
            row.update(i_ref_raw=i_ref, q_ref_raw=q_ref,
                       error_i_lsb=i_raw - i_ref, error_q_lsb=q_raw - q_ref)
            max_error = max(max_error, abs(i_raw - i_ref), abs(q_raw - q_ref))
        rows.append(row)

    with open(args.output, "w", newline="", encoding="utf-8") as output:
        writer = csv.DictWriter(output, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    if args.phase_start is not None:
        end_phase = (args.phase_start + args.samples * args.phase_step) & 0xFFFFFFFF
        print(f"Independent double-precision reference: max component error {max_error} LSB; "
              f"limit {args.tolerance_lsb} LSB; next --phase-start {end_phase}")
    complex_samples = [complex(row["i_value"], row["q_value"]) for row in rows]
    measured_amplitude = sum(abs(sample) for sample in complex_samples) / len(rows)
    print(f"Mean measured complex amplitude: {measured_amplitude:.6f}; "
          f"programmed amplitude magnitude: {abs(args.amplitude_raw) / 16384.0:.6f}")
    if args.sample_rate_hz:
        frequency = args.phase_step * args.sample_rate_hz / 4294967296.0
        cross = sum(a.conjugate() * b for a, b in
                    zip(complex_samples, complex_samples[1:]))
        measured_frequency = (math.atan2(cross.imag, cross.real) *
                              args.sample_rate_hz / (2.0 * math.pi)) if cross else float("nan")
        print(f"Nominal tone frequency: {frequency:.6f} Hz; "
              f"measured frequency: {measured_frequency:.6f} Hz")
    print(f"Captured {len(rows)} samples to {args.output}; "
          f"capture status=0x{state:08x}, HLS status=0x{hls_status:08x}")
    if args.phase_start is not None and max_error > args.tolerance_lsb:
        raise SystemExit("capture exceeds reference tolerance")


if __name__ == "__main__":
    main()
