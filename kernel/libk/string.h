#include <kernel/types.h>

extern "C" int memcmp(const void *s1, const void *s2, size_t n);
extern "C" void *memcpy(void *s1, const void *s2, size_t n);
extern "C" void *memmove(void *s1, const void *s2, size_t n);
extern "C" void *memset(void *s, int c, size_t n);

char *strcat(char *s1, const char *s2);
char *strchr(const char *s, int c);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *s1, const char *s2);
size_t strcspn(const char *s1, const char *s2);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t n);
char *strncat(char *s1, const char *s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *s1, const char *s2, size_t n);
char *strpbrk(const char *s1, const char *s2);
char *strrchr(const char *s, int c);
char *strrev(char *p);
char *strtok(char *s, const char *delim);
size_t strspn(const char *s1, const char *s2);
char *strstr(const char *s1, const char *s2);
