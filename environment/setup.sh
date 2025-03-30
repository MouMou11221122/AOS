#!/bin/bash

qemu-system-x86_64 \
  -enable-kvm \
  -cpu host \
  -smp 6 \
  -m 12288 \
  -kernel linux-6.6.84/arch/x86/boot/bzImage \
  -drive file=rootfs.ext4,format=raw,if=virtio \
  -drive file=disk1.img,format=qcow2,if=virtio \
  -drive file=disk2.img,format=qcow2,if=virtio \
  -append "root=/dev/vda console=ttyS0" \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 -device e1000,netdev=net0 \
  -nographic

# -netdev user,id=net0 -device e1000,netdev=net0 \

