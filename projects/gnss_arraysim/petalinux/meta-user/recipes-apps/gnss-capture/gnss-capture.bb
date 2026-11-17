SUMMARY = "GNSS ArraySim finite BRAM capture tools"
LICENSE = "CLOSED"
SRC_URI = "file://capture_tone.py file://capture_gaussian.py file://capture_prn.py file://chips-example.csv file://gnss-check"
S = "${WORKDIR}"
RDEPENDS:${PN} = "python3-core python3-mmap python3-math python3-io libiio-tests"
do_install() {
    install -d ${D}/opt/gnss-arraysim ${D}${bindir}
    install -m 0755 ${WORKDIR}/capture_*.py ${D}/opt/gnss-arraysim/
    install -m 0644 ${WORKDIR}/chips-example.csv ${D}/opt/gnss-arraysim/
    install -m 0755 ${WORKDIR}/gnss-check ${D}${bindir}/gnss-check
}
FILES:${PN} += "/opt/gnss-arraysim"
