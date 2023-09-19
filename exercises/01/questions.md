# Exercise 01 - Q&A

## 1. What did you accomplish in this exercise?
To set up the build environment, build a linux image for the PYNQ board using YOCTO, load it in the SD card and boot up the machine

## 2. How does Yocto recognize that the target machine "pynq" is supported?
Using the MACHINE variable, the layers added and the meta-pynq directory.

## 3. Where do you find the device-tree source code for the PYNQ board?
In the Xilinx repo and the repo provided by the course staff.

## 4. What is the priority of the PYNQ support layer?
5

## 5. What additional applications are installed to the `core-image-minimal` target in the PYNQ support layer?
