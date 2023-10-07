#!/bin/bash

cd /home/student/11/build/tmp/deploy/images/pynq

cp -L boot.bin \
	u-boot.img \
	uImage \
	core-image-minimal-pynq.cpio.gz.u-boot \
	uEnv.txt \
	devicetree.dtb \
	/media/student/PYNQ-ANDRWS

umount -l /media/student/PYNQ-ANDRWS



