## How to build kernel for i686 arch

1. Get toolchain from https://github.com/lordmilko/i686-elf-tools/releases
2. Unpack the toolchain into /opt directory
3. Go to kernel/ directory
4. Build kernel using `make ARCH_FAMILY="x86" ARCH="i686"`

## Run

```
qemu-system-i386 -kernel kernel.elf
```
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
