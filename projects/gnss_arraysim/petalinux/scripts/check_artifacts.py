#!/usr/bin/env python3
"""Check the built DTB, kernel configuration and bitstream before packaging."""
import hashlib
import pathlib
import struct
import sys
import zipfile

project = pathlib.Path(sys.argv[1])

def fail(message):
    sys.exit('Validación: ' + message)

# Minimal flattened-device-tree reader; no dtc or third-party Python required.
def read_dtb(path):
    data = path.read_bytes()
    magic, size, off_struct, off_strings = struct.unpack_from('>4I', data)
    if magic != 0xD00DFEED or size > len(data):
        fail('DTB inválido')
    pos, stack, nodes = off_struct, [], []
    while True:
        token, = struct.unpack_from('>I', data, pos)
        pos += 4
        if token == 1:
            end = data.index(b'\0', pos)
            name = data[pos:end].decode()
            pos = (end + 4) & ~3
            node = {'name': name, 'props': {}}
            nodes.append(node)
            stack.append(node)
        elif token == 2:
            stack.pop()
        elif token == 3:
            length, offset = struct.unpack_from('>2I', data, pos)
            pos += 8
            start = off_strings + offset
            name = data[start:data.index(b'\0', start)].decode()
            stack[-1]['props'][name] = data[pos:pos+length]
            pos = (pos + length + 3) & ~3
        elif token == 4:
            pass
        elif token == 9:
            return nodes
        else:
            fail(f'token DTB desconocido: {token}')

nodes = read_dtb(project / 'images/linux/system.dtb')
for addr in (0x87020000, 0x87050000, 0x87080000):
    matches = [n for n in nodes if n['name'].lower().endswith(f'@{addr:x}')]
    if not matches or any(n['props'].get('status') != b'disabled\0' for n in matches):
        fail(f'GPIO de captura {addr:#x} no está deshabilitado para uso exclusivo por /dev/mem')
compat = b'\0'.join(n['props'].get('compatible', b'') for n in nodes
                    if n['props'].get('status', b'okay\0') != b'disabled\0')
for driver in (b'adi,ad9361', b'adi,axi-ad9361-6.00.a', b'adi,axi-ad9361x2-dds-6.00.a', b'adi,axi-dmac-1.00.a'):
    if driver not in compat:
        fail(f'falta el dispositivo ADI {driver.decode()}')
phys = [n for n in nodes if b'adi,ad9361' in n['props'].get('compatible', b'')]
if len(phys) != 2:
    fail(f'se esperaban dos PHY AD9361; encontrados {len(phys)}')
configs = list(project.glob('build/tmp/work/*/linux-xlnx/*/*/.config'))
if not configs:
    fail('no se encuentra el .config del kernel compilado')
config = max(configs, key=lambda p: p.stat().st_mtime).read_text()
for symbol in ('DEVMEM', 'AD9361', 'CF_AXI_ADC', 'CF_AXI_DDS', 'AXI_DMAC'):
    if f'CONFIG_{symbol}=y\n' not in config:
        fail(f'falta CONFIG_{symbol}=y en el kernel final')
for symbol in ('STRICT_DEVMEM', 'IO_STRICT_DEVMEM'):
    if f'CONFIG_{symbol}=y\n' in config:
        fail(f'CONFIG_{symbol} impide el acceso MMIO previsto')
with zipfile.ZipFile(project / 'hw-description/system_top.xsa') as archive:
    bits = [archive.read(n) for n in archive.namelist() if n.endswith('.bit')]
    actual = (project / 'images/linux/system.bit').read_bytes()
    if not any(hashlib.sha256(b).digest() == hashlib.sha256(actual).digest() for b in bits):
        fail('system.bit no coincide con el bitstream del XSA importado')
print('DTB ADI/capturas, configuración del kernel y bitstream verificados.')
