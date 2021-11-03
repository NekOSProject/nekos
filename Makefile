# Makefile to build all system

# Currently default target is i686
all: i686-build-grub-iso

i686-build-grub-iso: iso-clean i686-build-kernel
	mkdir -p isodir/boot/grub
	cp kernel/kernel.elf isodir/boot/kernel.elf
	cp boot/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o HOS.iso isodir/

i686-build-kernel:
	make -C kernel/ -f Makefile ARCH_FAMILY="x86" ARCH="i686"

i686-clean-kernel:
	make -C kernel/ -f Makefile clean ARCH_FAMILY="x86" ARCH="i686"

iso-clean:
	rm -rf isodir/
	rm -rf *.iso