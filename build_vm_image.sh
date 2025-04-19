#!/bin/bash
set -e

ROOTFS_DIR=debian-root
ROOT_IMG=debian-root.img
SIZE=8G
KERNEL_VERSION=$(uname -r)

echo "[+] Bootstrap Debian to $ROOTFS_DIR..."
sudo debootstrap --arch=amd64 stable "$ROOTFS_DIR" http://deb.debian.org/debian

echo "[+] Setting root password (default: root)"
echo "root:root" | sudo chroot "$ROOTFS_DIR" chpasswd

echo "[+] Copying kernel modules from host..."
sudo mkdir -p "$ROOTFS_DIR/lib/modules"
sudo cp -r "/lib/modules/$KERNEL_VERSION" "$ROOTFS_DIR/lib/modules/"

echo "[+] Installing useful packages in chroot..."
sudo chroot "$ROOTFS_DIR" /bin/bash <<EOF
apt update
apt install -y libcapstone-dev build-essential vim gdb
EOF

echo "[+] Packing into image: $ROOT_IMG..."
sudo virt-make-fs --type=ext4 --size=$SIZE "$ROOTFS_DIR" "$ROOT_IMG"

echo "[✓] VM image ready: $ROOT_IMG"