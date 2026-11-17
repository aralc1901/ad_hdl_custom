#!/usr/bin/env bash
source "$(dirname -- "$0")/common.sh"
require_petalinux
[[ $# == 0 ]] || fail 'Uso: build.sh'
[[ -f $PROJECT_DIR/hw-description/SHA256SUMS ]] || fail 'Ejecuta primero prepare.sh.'
cd "$PROJECT_DIR"
sha256sum -c hw-description/SHA256SUMS
petalinux-build
"$PETA_DIR/scripts/package.sh"
