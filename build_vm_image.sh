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

echo "[+] Writing static DNS settings..."
cat <<EOF | sudo tee $ROOTFS_DIR/etc/resolv.conf > /dev/null
nameserver 8.8.8.8
nameserver 8.8.4.4
EOF

echo "[+] Configuring static network..."
sudo tee "$ROOTFS_DIR/etc/network/interfaces" > /dev/null <<EOF
auto lo
iface lo inet loopback

auto ens4
iface ens4 inet static
    address 10.0.2.15
    netmask 255.255.255.0
    gateway 10.0.2.2
EOF
# note that the interface name could vary

echo "[+] Installing useful packages in chroot..."
sudo chroot "$ROOTFS_DIR" /bin/bash <<EOF
apt update
apt install -y libcapstone-dev build-essential vim gdb wget python3 iputils-ping net-tools curl
EOF

echo "[+] Packing into image: $ROOT_IMG..."
sudo virt-make-fs --type=ext4 --size=$SIZE "$ROOTFS_DIR" "$ROOT_IMG"

echo "[✓] VM image ready: $ROOT_IMG"