#include "ports.h"

inline u8 inb(u16 port) {
	u8 ret;
	asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline u16 inw(u16 port) {
	u16 ret;
	asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline u32 inl(u16 port) {
	u32 ret;
	asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

inline void outb(u16 port, u8  val) {
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outw(u16 port, u16 val) {
	asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outl(u16 port, u32 val) {
	asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

inline void insl(u16 port, void *buffer, size_t count) {
	asm("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}

int com1_is_transmit_empty() {
   return inb(PORT_COM1 + 5) & 0x20;
}

void com1_write_char(char a) {
   while (com1_is_transmit_empty() == 0);
   outb(PORT_COM1, a);
}