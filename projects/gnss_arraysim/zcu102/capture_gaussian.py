#!/usr/bin/env python3
"""Configure one finite Gaussian noise capture through /dev/mem and save I/Q CSV.

Requires the matching bitstream and PL address map on ZCU102 Linux. This
program has not been run on the board; it performs no RF output.
"""

import argparse
import csv
import math
import os
import time

from capture_tone import Window, signed16

GAUSSIAN_BASE = 0x87030000
BRAM_BASE = 0x87040000
GPIO_BASE = 0x87050000


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--samples", type=int, default=1024)
    parser.add_argument("--amplitude-raw", type=lambda s: int(s, 0), required=True,
                        help="signed Q2.14 integer, e.g. 4096 for 0.25")
    parser.add_argument("--seed", type=lambda s: int(s, 0), required=True,
                        help="nonzero 32-bit seed; 0 continues the PRNG state")
    parser.add_argument("--timeout", type=float, default=5.0)
    parser.add_argument("--output", default="gaussian_capture.csv")
    return parser.parse_args()


def main():
    args = parse_args()
    if not 1 <= args.samples <= 1024:
        raise SystemExit("--samples must be in 1..1024")
    if not -32768 <= args.amplitude_raw <= 32767:
        raise SystemExit("--amplitude-raw must be a signed 16-bit integer")
    if not 0 <= args.seed <= 0xFFFFFFFF:
        raise SystemExit("--seed must be an unsigned 32-bit integer")
    if args.timeout <= 0:
        raise SystemExit("--timeout must be positive")

    fd = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
    gaussian, bram, gpio = (Window(fd, address) for address in
                            (GAUSSIAN_BASE, BRAM_BASE, GPIO_BASE))
    try:
        gpio.write(0x04, 0x00000000)  # channel 1: output
        gpio.write(0x0C, 0xFFFFFFFF)  # channel 2: input
        gpio.write(0x00, 0)

        gaussian.write(0x00, 0)  # auto_restart=0
        if not gaussian.read(0x00) & 0x4:
            raise RuntimeError("Gaussian generator is not idle; reset/reload the FPGA")
        gaussian.write(0x10, args.samples)
        gaussian.write(0x18, args.amplitude_raw & 0xFFFF)
        gaussian.write(0x20, args.seed)

        control = args.samples << 16
        gpio.write(0x00, control)
        gpio.write(0x00, control | 1)  # ARM rising edge
        gpio.write(0x00, control)
        state = gpio.read(0x08)
        if state & 0x6000 or not state & 0x1000:
            raise RuntimeError(f"capture did not arm: status=0x{state:08x}")

        gaussian.write(0x00, 1)  # ap_start
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
        hls_status = gaussian.read(0x00)  # ap_done is clear-on-read
    finally:
        gaussian.close()
        bram.close()
        gpio.close()
        os.close(fd)

    rows = []
    for index, word in enumerate(words):
        i_raw, q_raw = signed16(word), signed16(word >> 16)
        rows.append({"sample": index, "word_hex": f"0x{word:08x}",
                     "i_raw": i_raw, "q_raw": q_raw,
                     "i_value": i_raw / 16384.0, "q_value": q_raw / 16384.0})
    with open(args.output, "w", newline="", encoding="utf-8") as output:
        writer = csv.DictWriter(output, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    samples = [complex(row["i_value"], row["q_value"]) for row in rows]
    mean = sum(samples) / len(samples)
    rms = math.sqrt(sum(abs(sample) ** 2 for sample in samples) / len(samples))
    if len(samples) > 1:
        lag1 = sum(a.conjugate() * b for a, b in
                   zip(samples, samples[1:])) / (len(samples) - 1)
        print(f"Lag-1 complex correlation: {lag1.real:.6f} + j{lag1.imag:.6f}")
    print(f"Mean I/Q: {mean.real:.6f}, {mean.imag:.6f}; complex RMS: {rms:.6f}; "
          f"programmed |amplitude|: {abs(args.amplitude_raw) / 16384.0:.6f}")
    print(f"Captured {len(rows)} samples to {args.output}; "
          f"capture status=0x{state:08x}, HLS status=0x{hls_status:08x}")


if __name__ == "__main__":
    main()
