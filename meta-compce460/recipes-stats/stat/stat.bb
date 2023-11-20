SUMMARY = "Script and data to test the evil module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

SRC_URI = "file://stat \
           file://COPYING \
          "

do_configure[noexec] = "1"
do_compile[noexec] = "1"

S = "${WORKDIR}"

do_install() {
	install -Dm 0744 ${S}/stat ${D}/opt/stat/sbin/stat
}

FILES_${PN} += "/opt/stat"

