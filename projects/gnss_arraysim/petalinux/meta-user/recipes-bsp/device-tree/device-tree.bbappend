FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append = " file://isolate-capture.py file://system-user.dtsi file://system-conf.dtsi"

do_configure:append() {
    # DTG writes pl.dtsi here, but user files are unpacked into WORKDIR.
    # Install both before isolation and meta-adi's system-conf processing.
    install -m 0644 ${WORKDIR}/system-user.dtsi ${DT_FILES_PATH}/system-user.dtsi
    install -m 0644 ${WORKDIR}/system-conf.dtsi ${DT_FILES_PATH}/system-conf.dtsi
    python3 ${WORKDIR}/isolate-capture.py ${DT_FILES_PATH}/pl.dtsi ${DT_FILES_PATH}/system-user.dtsi
}
