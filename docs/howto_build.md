## How to build kernel for i686 arch
### On Linux

1. Get [i686-elf toolchain](https://github.com/lordmilko/i686-elf-tools/releases)
2. Unpack the toolchain into /opt directory
3. Go to kernel/ directory
4. Build kernel using `make ARCH_FAMILY="x86" ARCH="i686"`

### On Windows

1. Get [clang](https://llvm.org/builds/)
2. Run build.py file (`python3 build.py`)

## How to run the kernel
### On Linux

```
qemu-system-i386 -kernel kernel.elf
```

### On Windows:

On Windows Qemu can say something like "can't find bios image". In this case this should work:
```
qemu-system-i386 -L C:\Path\to\qemu\installation\folder -kernel kernel.elf
```

## How to run tests

```
python3 runtests.py
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
