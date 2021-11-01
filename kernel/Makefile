CROSS_СС=$(ARCH)-elf-gcc
NASM=nasm

KERNEL_CFLAGS=-std=gnu11 -ffreestanding -Wall -Wextra -g
KERNEL_LDFLAGS=-ffreestanding -nostdlib -lgcc

KERNEL_OBJS =  $(filter-out arch/*, $(patsubst %.c,%.o,$(wildcard *.c)))
KERNEL_OBJS += $(filter-out arch/*, $(patsubst %.c,%.o,$(wildcard */*.c)))
KERNEL_HEADERS = $(filter-out arch/*, $(wildcard *.h))
KERNEL_HEADERS += $(filter-out arch/*, $(wildcard */*.h))

KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard arch/$(ARCH_FAMILY)/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard arch/$(ARCH_FAMILY)/$(ARCH)/*.c))
KERNEL_ASMOBJS = $(patsubst %.asm,%.o,$(wildcard arch/$(ARCH_FAMILY)/$(ARCH)/*.asm))
KERNEL_HEADERS += $(wildcard arch/$(ARCH_FAMILY)/*.h)
KERNEL_HEADERS += $(wildcard arch/$(ARCH_FAMILY)/$(ARCH)/*.h)

#

%.o: %.asm
	$(NASM) -f elf32 $< -o $@

%.o: %.c $(KERNEL_HEADERS)
	$(CROSS_СС) $(KERNEL_CFLAGS) -I../ -c -o $@ $<

all: $(KERNEL_OBJS) $(KERNEL_ASMOBJS)
	$(info $(KERNEL_OBJS))
	$(info $(KERNEL_HEADERS))

	$(CROSS_СС) $(KERNEL_LDFLAGS) -T arch/$(ARCH_FAMILY)/$(ARCH)/link.ld -o kernel.elf $(KERNEL_ASMOBJS) $(KERNEL_OBJS)

clean:
	-rm -f kernel.elf
	-rm -f ${KERNEL_ASMOBJS}
	-rm -f ${KERNEL_OBJS}
	