#include "string.h"

/* This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will. */

// Memory functions

int memcmp(const void *s1, const void *s2, size_t n) {
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        ++p1;
        ++p2;
    }
    return 0;
}

extern "C" void *memcpy(void *s1, const void *s2, size_t n) {
    char *dest = (char *)s1;
    const char *src = (const char *)s2;
    while (n--) {
        *dest++ = *src++;
    }
    return s1;
}

void *memmove(void *s1, const void *s2, size_t n) {
    char *dest = (char *)s1;
    const char *src = (const char *)s2;
    if (dest <= src) {
        while (n--) {
            *dest++ = *src++;
        }
    } else {
        src += n;
        dest += n;
        while (n--) {
            *--dest = *--src;
        }
    }
    return s1;
}

void *memset(void *s, int c, size_t n) {
    u8 *p = (u8 *)s;
    while (n--) {
        *p++ = (u8)c;
    }
    return s;
}

// String functions

char *strcat(char *s1, const char *s2) {
    char *rc = s1;
    if (*s1) {
        while (*++s1) {
            /* EMPTY */
        }
    }
    while ((*s1++ = *s2++)) {
        /* EMPTY */
    }
    return rc;
}

char *strchr(const char *s, int c) {
    do {
        if (*s == (char)c) {
            return (char *)s;
        }
    } while (*s++);

    return NULL;
}

int strcmp(const char *s1, const char *s2) {
    while ((*s1) && (*s1 == *s2)) {
        ++s1;
        ++s2;
    }
    return (*(u8 *)s1 - *(u8 *)s2);
}

char *strcpy(char *s1, const char *s2) {
    char *rc = s1;
    while ((*s1++ = *s2++)) {
        /* EMPTY */
    }
    return rc;
}

size_t strcspn(const char *s1, const char *s2) {
    size_t len = 0;
    const char *p;
    while (s1[len]) {
        p = s2;
        while (*p) {
            if (s1[len] == *p++) {
                return len;
            }
        }
        ++len;
    }
    return len;
}

size_t strlen(const char *s) {
    size_t rc = 0;
    while (s[rc]) {
        ++rc;
    }
    return rc;
}

char *strncat(char *s1, const char *s2, size_t n) {
    char *rc = s1;
    while (*s1) {
        ++s1;
    }
    while (n && (*s1++ = *s2++)) {
        --n;
    }
    if (n == 0) {
        *s1 = '\0';
    }
    return rc;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0) {
        return 0;
    } else {
        return (*(u8 *)s1 - *(u8 *)s2);
    }
}

char *strncpy(char *s1, const char *s2, size_t n) {
    char *rc = s1;
    while (n && (*s1++ = *s2++)) {
        /* Cannot do "n--" in the conditional as size_t is unsigned and we have
           to check it again for >0 in the next loop below, so we must not risk
           underflow.
        */
        --n;
    }
    /* Checking against 1 as we missed the last --n in the loop above. */
    while (n-- > 1) {
        *s1++ = '\0';
    }
    return rc;
}

char *strpbrk(const char *s1, const char *s2) {
    const char *p1 = s1;
    const char *p2;
    while (*p1) {
        p2 = s2;

        while (*p2) {
            if (*p1 == *p2++) {
                return (char *)p1;
            }
        }
        ++p1;
    }
    return NULL;
}

char *strrchr(const char *s, int c) {
    size_t i = 0;
    while (s[i++]) {
        /* EMPTY */
    }
    do {
        if (s[--i] == (char)c) {
            return (char *)s + i;
        }
    } while (i);
    return NULL;
}

char *strrev(char *p) {
    char *q = p, *res = p, z;
    while (q && *q)
        ++q; /* find eos */
    for (--q; p < q; ++p, --q) {
        z = *p;
        *p = *q;
        *q = z;
    }
    return res;
}

char *strtok(char *s, const char *delim) {
    static char *savep;
    char *res;
    if (s)
        savep = NULL;
    else
        s = savep;
    if (*s == '\0')
        return NULL;
    s += strspn(s, delim);
    if (*s == '\0')
        return NULL;
    res = s;
    s += strcspn(s, delim);
    savep = s + 1;
    *s = '\0';
    return res;
}

size_t strspn(const char *s1, const char *s2) {
    size_t len = 0;
    const char *p;
    while (s1[len]) {
        p = s2;
        while (*p) {
            if (s1[len] == *p) {
                break;
            }
            ++p;
        }
        if (!*p) {
            return len;
        }
        ++len;
    }
    return len;
}

char *strstr(const char *s1, const char *s2) {
    const char *p1 = s1;
    const char *p2;
    while (*s1) {
        p2 = s2;
        while (*p2 && (*p1 == *p2)) {
            ++p1;
            ++p2;
        }
        if (!*p2) {
            return (char *)s1;
        }
        ++s1;
        p1 = s1;
    }
    return NULL;
}
