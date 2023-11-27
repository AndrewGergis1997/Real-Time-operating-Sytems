#!/bin/bash

cd ~/11/build/tmp/deploy/sdk

chmod +x poky-glibc-x86_64-core-image-minimal-cortexa9hf-neon-toolchain-2.4.3.sh

./poky-glibc-x86_64-core-image-minimal-cortexa9hf-neon-toolchain-2.4.3.sh -d ~/opt/poky-sdk/2.4.3

source ~/opt/poky-sdk/2.4.3/environment-setup-cortexa9hf-neon-poky-linux-gnueabi
