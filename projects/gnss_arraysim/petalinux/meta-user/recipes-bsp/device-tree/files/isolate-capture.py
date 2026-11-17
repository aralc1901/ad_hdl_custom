#!/usr/bin/env python3
"""Prevent generated GPIO/HLS/BRAM drivers from claiming capture MMIO/IRQs."""
import pathlib
import re
import sys
pl, user = map(pathlib.Path, sys.argv[1:])
text = pl.read_text()
# DTG emits a label and unit address for memory-mapped IPs. Work by address,
# not guessed labels, and fail if the required three GPIOs are absent.
nodes = re.findall(r'\b([A-Za-z_]\w*):\s*[^\s{};]+@([0-9a-fA-F]+)\s*\{', text)
labels = {int(addr, 16): label for label, addr in nodes
          if 0x87000000 <= int(addr, 16) <= 0x87090000}
for addr in (0x87020000, 0x87050000, 0x87080000):
    if addr not in labels:
        sys.exit(f'No se encuentra el GPIO de captura {addr:#x} en pl.dtsi; revisar XSA/DTG.')
content = user.read_text().split('/* BEGIN GNSS MMIO */')[0].rstrip()
content += '\n\n/* BEGIN GNSS MMIO */\n'
for addr, label in sorted(labels.items()):
    content += f'&{label} {{ status = "disabled"; }}; /* {addr:#x}: /dev/mem */\n'
user.write_text(content)
