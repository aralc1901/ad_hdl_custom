#!/usr/bin/env python3
"""Reject BD-only exports, wrong devices and incompatible GNSS maps."""
import sys
import zipfile
import xml.etree.ElementTree as ET

with zipfile.ZipFile(sys.argv[1]) as archive:
    names = archive.namelist()
    if not any(n.endswith('.bit') for n in names):
        sys.exit('El XSA no incluye bitstream; exporta el build completo con timing correcto.')
    hw = [n for n in names if n.endswith('.hwh')]
    if not hw:
        sys.exit('XSA sin descripción HWH.')
    roots = [ET.fromstring(archive.read(n)) for n in hw]
    text = '\n'.join(archive.read(n).decode() for n in hw).lower()
    if 'xczu9eg' not in text:
        sys.exit('El XSA no identifica el dispositivo XCZU9EG de ZCU102.')
    expected = {
        'jam_tone_0': 0x87000000, 'tone_capture_axi_bram': 0x87010000,
        'tone_capture_gpio': 0x87020000, 'jam_gaussian_0': 0x87030000,
        'gaussian_capture_axi_bram': 0x87040000, 'gaussian_capture_gpio': 0x87050000,
        'gnss_prn_0': 0x87060000, 'prn_capture_axi_bram': 0x87070000,
        'prn_capture_gpio': 0x87080000, 'fp_arraygen_0': 0x87090000,
        'axi_ad9361_0': 0x99020000, 'axi_ad9361_1': 0x99040000,
        'axi_ad9361_adc_dma': 0x9C400000, 'axi_ad9361_dac_dma': 0x9C420000,
    }
    ranges = [e.attrib for root in roots for e in root.iter()
              if e.tag.split('}')[-1] == 'MEMRANGE']
    for name, addr in expected.items():
        if not any(r.get('INSTANCE') == name and int(r.get('BASEVALUE', '0'), 0) == addr for r in ranges):
            sys.exit(f'El mapa PS del XSA no contiene {name} en {addr:#x}.')
print('XSA: bitstream, dispositivo y mapa PS correctos.')
