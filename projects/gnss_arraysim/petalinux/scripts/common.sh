#!/usr/bin/env bash
set -euo pipefail
PETA_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
WORK_DIR=${GNSS_PETA_WORK:-$PETA_DIR/work}
PROJECT_DIR=$WORK_DIR/gnss-arraysim-zcu102
META_ADI=$WORK_DIR/meta-adi
META_ADI_REV=3a5de5cf8f1cf4a0fe39a26cbcc6a292a0e5b654
DEFAULT_XSA=$PETA_DIR/../zcu102/gnss_arraysim_zcu102.sdk/system_top.xsa
fail() { echo "ERROR: $*" >&2; exit 1; }
require_petalinux() {
    command -v petalinux-config >/dev/null || fail 'Carga PetaLinux 2022.2/settings.sh o entra en el contenedor 2022.2.'
    [[ ${PETALINUX_VER:-} == 2022.2* ]] || fail "Se requiere PETALINUX_VER=2022.2; detectado: ${PETALINUX_VER:-sin definir}"
    [[ $EUID != 0 ]] || fail 'PetaLinux/Yocto debe ejecutarse como usuario normal.'
}
