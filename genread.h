#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define CBUF 2
#define WABUF 1
#define LBUF 2

typedef unsigned char boole;
typedef enum
{
    UNK, /* unknown type, so default to string */
    NUM /* number: treat as float, an int requires more information */
} t_t;

typedef struct /* word type */
{
    char *w;
    t_t t; /* number or not */
    unsigned lp1; /* length of word plus one (i.e. includes null char */
} w_c;

typedef struct /* aw_c: array of words container */
{
    w_c **aw;
    unsigned ab;
    unsigned al;
} aw_c;

typedef struct /* aaw_c: array of array of words container */
{
    size_t numl; /* number of lines, i.e. rows */
    aw_c **aaw; /* an array of pointers to aw_c */
} aaw_c;

