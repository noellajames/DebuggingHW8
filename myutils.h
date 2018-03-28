#ifndef _MYUTILS_H
#define _MYUTILS_H

unsigned my_strlen(char *);

int my_strcmp(char*, char*);

int my_strncmp(char*, char*, unsigned);

void my_memset(char *, char, unsigned);

void my_memcpy(char *, char *, unsigned);

int my_memcmp(char *, char *, unsigned);

int my_atoi(char s[]);

int my_isdigit(char c);

int my_isspace(char c);

#endif