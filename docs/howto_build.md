## How to build kernel
Go to kernel/ directory. To build kernel for i686 run:
```
make ARCH_FAMILY="x86" ARCH="i686"
```
You will get kernel.elf file here. To run kernel directly in qemu use:
```
qemu-system-i386 -kernel kernel.elf
```
You can get prebuilt toolchain for i686 here: https://github.com/lordmilko/i686-elf-tools/releases
## How to build iso image
For x86 i686:
Install following packages:
```
sudo apt-get update
sudo apt-get install grub-common
sudo apt-get install xorriso
sudo apt-get install grub-pc-bin
```
Run:
```
make i686-build-grub-iso
```
