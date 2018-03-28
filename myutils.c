#include <stdio.h>
#include <stdlib.h>

#include "myutils.h"

unsigned my_strlen(char* s) {
    char *p = s;
    while (*p != '\0')
        p++;
    return p - s;
}

int my_strcmp(char* s, char* t) {
    for ( ; *s == *t; s++, t++) {
        if (*s == '\0') {
            return 0;
        }
    }
    return *s - *t;
}

int my_strncmp(char* s, char* t, unsigned len) {
    unsigned i = 0;
    for (; *s == *t; s++, t++) {
        if (*s == '\0') {
            return 0;
        }
        i++;
        if (i >= len) {
            break;
        }
    }
    return *s - *t;
}

void my_memset(char * s, char c, unsigned len) {
    for (unsigned i = 0; i < len; ++i)
        s[i] = c;
}

void my_memcpy(char * src, char * dest, unsigned len) {
    for (unsigned i = 0; i < len; ++i )
        dest[i] = src[i];
}

int my_memcmp(char * s, char * t, unsigned len) {
    for (unsigned i = 0 ;  i < len; ++i) {
        if (s[i] == t[i]) {
            continue;
        }
        return s[i] - t[i];
    }
    return 0;
}

int my_isdigit(char c) {
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

int my_isspace(char c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'  || c == '\r') {
        return 1;
    }
    return 0;
}

/* atoi: convert s to integer */ 
int my_atoi(char s[])
{
    int i, n, sign;
    for (i = 0; my_isspace(s[i]); i++) /* skip white space */ 
        ;

    sign = (s[i] == '-') ? -1 : 1;
    
    if (s[i] == '+' || s[i] == '-') {/* skip sign */
        i++;
    }

    for (n = 0; my_isdigit(s[i]); i++) { 
        n = 10 * n + (s[i] - '0');
    }
    
    return sign * n;
}
