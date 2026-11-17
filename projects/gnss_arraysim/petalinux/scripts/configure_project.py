#!/usr/bin/env python3
"""Apply an idempotent, managed configuration block to the generated project."""
import pathlib
import re
import sys
project, adi = map(pathlib.Path, sys.argv[1:])
config = project / 'project-spec/configs/config'
values = {
    'CONFIG_USER_LAYER_0': f'"{adi}/meta-adi-core"',
    'CONFIG_USER_LAYER_1': f'"{adi}/meta-adi-xilinx"',
    'CONFIG_SUBSYSTEM_ROOTFS_INITRAMFS': 'y',
    'CONFIG_SUBSYSTEM_ROOTFS_INITRD': None,
    'CONFIG_SUBSYSTEM_ROOTFS_EXT4': None,
    'CONFIG_SUBSYSTEM_ROOTFS_JFFS2': None,
    'CONFIG_SUBSYSTEM_ROOTFS_UBIFS': None,
    'CONFIG_SUBSYSTEM_ROOTFS_NFS': None,
    'CONFIG_SUBSYSTEM_ROOTFS_OTHER': None,
    'CONFIG_SUBSYSTEM_HOSTNAME': '"gnss-arraysim"',
    'CONFIG_SUBSYSTEM_EXTRA_BOOTARGS': '"clk_ignore_unused"',
    'CONFIG_SUBSYSTEM_FPGA_MANAGER': None,
    'CONFIG_SUBSYSTEM_DEVICETREE_OVERLAY': None,
}
text = config.read_text()
for key, value in values.items():
    text = re.sub(rf'^(?:{key}=.*|# {key} is not set)\n?', '', text, flags=re.M)
    text += f'{key}={value}\n' if value is not None else f'# {key} is not set\n'
config.write_text(text)
conf = project / 'project-spec/meta-user/conf/petalinuxbsp.conf'
text = conf.read_text() if conf.exists() else ''
text = re.sub(r'\n?# BEGIN GNSS ARRAYSIM\n.*?# END GNSS ARRAYSIM\n?', '', text, flags=re.S)
text += '''
# BEGIN GNSS ARRAYSIM
KERNEL_DTB = "zynqmp-zcu102-rev10-ad9361-fmcomms5"
# meta-adi defaults to AUTOREV: pin the release kernel and synchronization library.
SRCREV:pn-linux-xlnx = "cd7e20c430dc19df7c32610e9d5b494d8f313e07"
SRCREV:pn-libad9361-iio = "2bf936d6afcb3e6385642b6a769fd74f8ab9b62d"
IMAGE_INSTALL:append = " gnss-capture python3-core python3-mmap python3-math python3-io libiio libiio-tests libiio-iiod libad9361-iio openssh-sftp-server"
EXTRA_IMAGE_FEATURES:append = " ssh-server-dropbear"
# Remove unrelated applications added by meta-adi's generic minimal image.
IMAGE_INSTALL:remove = "fru-tools jesd-status adrv9009-zu11eg-fan-control"
# END GNSS ARRAYSIM
'''
conf.write_text(text)
