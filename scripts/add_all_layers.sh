#!/bin/bash

bitbake-layers add-layer meta-pynq
bitbake-layers add-layer meta-xilinx/meta-xilinx-bsp
bitbake-layers add-layer meta-xilinx/meta-xilinx-contrib
bitbake-layers add-layer meta-group11
