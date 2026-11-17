#!/usr/bin/env bash
source "$(dirname -- "$0")/common.sh"
require_petalinux
[[ $# == 0 ]] || fail 'Uso: package.sh'
cd "$PROJECT_DIR"
for file in zynqmp_fsbl.elf pmufw.elf bl31.elf u-boot.elf system.bit image.ub boot.scr system.dtb; do
    [[ -s images/linux/$file ]] || fail "Falta images/linux/$file; ejecuta build.sh."
done
python3 "$PETA_DIR/scripts/check_artifacts.py" "$PROJECT_DIR"
petalinux-package --boot --fsbl images/linux/zynqmp_fsbl.elf \
    --pmufw images/linux/pmufw.elf --fpga images/linux/system.bit \
    --atf images/linux/bl31.elf --u-boot images/linux/u-boot.elf --force
# Fresh directory per build avoids mixing old and new SD artifacts.
OUT=$PETA_DIR/artifacts/$(date -u +%Y%m%dT%H%M%SZ)
mkdir -p "$OUT"
cp images/linux/{BOOT.BIN,image.ub,boot.scr,system.dtb} "$OUT/"
cp hw-description/SHA256SUMS "$OUT/XSA-SHA256SUMS"
git -C "$META_ADI" rev-parse HEAD > "$OUT/meta-adi-revision.txt"
(cd "$OUT" && sha256sum BOOT.BIN image.ub boot.scr system.dtb > SHA256SUMS)
printf 'Archivos para la partición FAT de la SD: %s\n' "$OUT"
