#!/usr/bin/env bash
source "$(dirname -- "$0")/common.sh"
require_petalinux
[[ $# -le 1 ]] || fail 'Uso: prepare.sh [ruta/system_top.xsa]'
XSA=$(realpath -- "${1:-$DEFAULT_XSA}")
[[ -f $XSA ]] || fail "No existe el XSA: $XSA. Termina primero el build HDL completo."
python3 "$PETA_DIR/scripts/validate_xsa.py" "$XSA"
mkdir -p "$WORK_DIR"
if [[ ! -d $META_ADI ]]; then
    git clone --no-checkout https://github.com/analogdevicesinc/meta-adi.git "$META_ADI"
    git -C "$META_ADI" checkout --detach "$META_ADI_REV"
fi
[[ $(git -C "$META_ADI" rev-parse HEAD) == "$META_ADI_REV" ]] || fail 'La revisión de meta-adi no coincide con la fijada.'
[[ -z $(git -C "$META_ADI" status --porcelain) ]] || fail 'meta-adi tiene modificaciones locales.'
if [[ ! -d $PROJECT_DIR ]]; then
    (cd "$WORK_DIR" && petalinux-create -t project --template zynqMP --name gnss-arraysim-zcu102)
fi
cd "$PROJECT_DIR"
mkdir -p hw-description
cp -- "$XSA" hw-description/system_top.xsa
petalinux-config --get-hw-description="$PROJECT_DIR/hw-description" --silentconfig
cp -R "$PETA_DIR/meta-user/." project-spec/meta-user/
# Keep the three Python files together: Gaussian and PRN import capture_tone.
cp "$PETA_DIR"/../zcu102/capture_{tone,gaussian,prn}.py project-spec/meta-user/recipes-apps/gnss-capture/files/
python3 "$PETA_DIR/scripts/configure_project.py" "$PROJECT_DIR" "$META_ADI"
petalinux-config --silentconfig
sha256sum hw-description/system_top.xsa > hw-description/SHA256SUMS
printf 'Proyecto preparado: %s\nSiguiente: %s/scripts/build.sh\n' "$PROJECT_DIR" "$PETA_DIR"
