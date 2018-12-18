#include "strings.h"

void *memset(void *s, int c, size_t n)
{
    /* TODO: Your code here */
    return NULL;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    /* TODO: Your code here */
    return NULL;
}

int strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

int strcmp(const char *s1, const char *s2)
{
    /* TODO: Your code here */
    return 0;
}

int strlcat(char *dst, const char *src, int maxsize)
{
    /* TODO: Your code here */
    return 0;
}

unsigned int strtonum(const char *str, const char **endptr)
{
    /* TODO: Your code here */
    return 0;
}
