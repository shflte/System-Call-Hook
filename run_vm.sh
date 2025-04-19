#!/bin/bash

IMG=debian-root.img
SHARE_PATH=$(pwd)
KERNEL=/boot/vmlinuz-$(uname -r)

sudo qemu-system-x86_64 \
  -m 2048 \
  -nographic \
  -kernel "$KERNEL" \
  -initrd /boot/initrd.img-$(uname -r) \
  -append "root=/dev/vda rw console=ttyS0 nokaslr" \
  -drive file="$IMG",format=raw,if=virtio \
  -fsdev local,id=shared_dev,path="$SHARE_PATH",security_model=none \
  -device virtio-9p-pci,fsdev=shared_dev,mount_tag=hostshare