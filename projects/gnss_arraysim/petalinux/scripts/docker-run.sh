#!/usr/bin/env bash
source "$(dirname -- "$0")/common.sh"
[[ $# -ge 1 ]] || fail 'Uso: docker-run.sh prepare [XSA] | build | package | shell'
REPO=$(cd "$PETA_DIR/../../.." && pwd)
IMAGE=${GNSS_PETA_IMAGE:-docker_petalinux2:2022.2}
ACTION=$1
shift
case "$ACTION" in
    prepare|build|package) COMMAND=("$PETA_DIR/scripts/$ACTION.sh" "$@");;
    shell) COMMAND=(bash -l);;
    *) fail "Acción desconocida: $ACTION";;
esac
# Do not auto-select another release or run privileged. All build inputs must
# reside under the mounted repository (or be copied there before invocation).
docker run --rm -it --network host --user petalinux \
    -v "$REPO:$REPO" -w "$PETA_DIR" \
    --entrypoint /bin/bash "$IMAGE" -lc 'exec "$@"' bash "${COMMAND[@]}"
