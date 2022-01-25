rm -rf isodir/
rm -rf *.iso
mkdir -p isodir/boot/grub
cp kernel/kernel.elf isodir/boot/kernel.elf
cp boot/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o NekOS.iso isodir/