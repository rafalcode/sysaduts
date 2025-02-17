/* modification of matread but operating on words instead of floats */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "pread.h"

w_c *crea_wc(unsigned initsz)
{
    w_c *wc=malloc(sizeof(w_c));
    wc->lp1=initsz;
    wc->t=STRG;
    wc->w=malloc(wc->lp1*sizeof(char));
    return wc;
}

void reall_wc(w_c **wc, unsigned *cbuf)
{
    w_c *twc=*wc;
    unsigned tcbuf=*cbuf;
    tcbuf += CBUF;
    twc->lp1=tcbuf;
    twc->w=realloc(twc->w, tcbuf*sizeof(char));
    *wc=twc; /* realloc can often change the ptr */
    *cbuf=tcbuf;
    return;
}

void norm_wc(w_c **wc)
{
    w_c *twc=*wc;
    twc->w=realloc(twc->w, twc->lp1*sizeof(char));
    *wc=twc; /* realloc can often change the ptr */
    return;
}

void free_wc(w_c **wc)
{
    w_c *twc=*wc;
    free(twc->w);
    free(twc);
    return;
}

aw_c *crea_awc(unsigned initsz)
{
    int i;
    aw_c *awc=malloc(sizeof(aw_c));
    awc->ab=initsz;
    awc->al=awc->ab;
    awc->stsps=0;
    awc->sttbs=0;
    awc->aw=malloc(awc->ab*sizeof(w_c*));
    for(i=0;i<awc->ab;++i) 
        awc->aw[i]=crea_wc(CBUF);
    return awc;
}

void reall_awc(aw_c **awc, unsigned buf)
{
    int i;
    aw_c *tawc=*awc;
    tawc->ab += buf;
    tawc->al=tawc->ab;
    tawc->stsps=0;
    tawc->sttbs=0;
    tawc->aw=realloc(tawc->aw, tawc->ab*sizeof(aw_c*));
    for(i=tawc->ab-buf;i<tawc->ab;++i)
        tawc->aw[i]=crea_wc(CBUF);
    *awc=tawc;
    return;
}

void norm_awc(aw_c **awc)
{
    int i;
    aw_c *tawc=*awc;
    /* free the individual w_c's */
    for(i=tawc->al;i<tawc->ab;++i) 
        free_wc(tawc->aw+i);
    /* now release the pointers to those freed w_c's */
    tawc->aw=realloc(tawc->aw, tawc->al*sizeof(aw_c*));
    *awc=tawc;
    return;
}

void free_awc(aw_c **awc)
{
    int i;
    aw_c *tawc=*awc;
    for(i=0;i<tawc->al;++i) 
        free_wc(tawc->aw+i);
    free(tawc->aw); /* unbelieveable: I left this out, couldn't find where I leaking the memory! */
    free(tawc);
    return;
}

aaw_c *crea_aawc(unsigned initsz)
{
    int i;
    unsigned lbuf=initsz;
    aaw_c *aawc=malloc(sizeof(aaw_c));
    aawc->numl=0;
    aawc->aaw=malloc(lbuf*sizeof(aw_c*));
    for(i=0;i<initsz;++i) 
        aawc->aaw[i]=crea_awc(WABUF);
    /* ppa, this guys ia bit independent: will use CBUF as buffer increaser */
    aawc->ppb=CBUF;
    aawc->ppsz=0;
    aawc->ppa=malloc(CBUF*sizeof(int));
    return aawc;
}

void free_aawc(aaw_c **aw)
{
    int i;
    aaw_c *taw=*aw;
    for(i=0;i<taw->numl;++i) /* tried to release 1 more, no go */
        free_awc(taw->aaw+i);
    free(taw->ppa);
    free(taw->aaw);
    free(taw);
}

void prtaawapap(aaw_c *aawc) /* print aaw As Pure As Possible */
{
    int i, j, k, ppi=0;
    for(i=0;i<aawc->numl;++i) {
        // printf("ln%dsp%dtb%d) ", i, aawc->aaw[i]->stsps, aawc->aaw[i]->sttbs);
        /* order fo tabs and space will be messed up sure, but usually it will be one or the other. */
        for(j=0; j<aawc->aaw[i]->stsps;j++)
            putchar(' ');
        for(j=0; j<aawc->aaw[j]->sttbs;j++)
            putchar('\t');
        for(j=0;j<aawc->aaw[i]->al;++j) {
            for(k=0;k<aawc->aaw[i]->aw[j]->lp1-1; k++)
                putchar(aawc->aaw[i]->aw[j]->w[k]);
            if(j==aawc->aaw[i]->al-1)
                putchar('\n');
            else
                putchar(' ');
        }

        if( (ppi< aawc->ppsz) && (i == aawc->ppa[ppi])) { /* && means it will not evaluate second, if first is neg. */
            putchar('\n');
            printf("%d ", ppi); 
            ppi++;
        }
    }
}

void prtaawcdata(aaw_c *aawc) /* print line and word details, but not the words themselves */
{
    int i, j;
    for(i=0;i<aawc->numl;++i) {
        // printf("L%u(%uw):", i, aawc->aaw[i]->al); 
        for(j=0;j<aawc->aaw[i]->al;++j) {
            //printf("l%ut", aawc->aaw[i]->aw[j]->lp1-1);
            switch(aawc->aaw[i]->aw[j]->t) {
                case NUM: printf("N:"); break;
                case STRG: printf("S:"); break; /* basic string */
                case STES: printf("E:"); break; /* word is a string and ends with a period */
                case STPU: printf("P:"); break; /* closing punctuation */
                case STEF: printf("F:"); break; /* closing para */
            }
            printf("%s ", aawc->aaw[i]->aw[j]->w);
        }
        printf("(%uw)\n", aawc->aaw[i]->al); 
    }
    printf("\n"); 
}

void prtaawcdat2(aaw_c *aawc) /* print line and word details, but not the words themselves */
{
    int i, j;
    for(i=0;i<aawc->numl;++i) {
        // printf("L%u(%uw):", i, aawc->aaw[i]->al); 
        for(j=0;j<aawc->aaw[i]->al;++j) {
            //printf("l%ut", aawc->aaw[i]->aw[j]->lp1-1);
            switch(aawc->aaw[i]->aw[j]->t) {
                case NUM: printf("N:"); break;
                case STRG: printf("S:"); break; /* basic string */
                case STES: printf("E:"); break; /* word is a string and ends with a period */
                case STPU: printf("P:"); break; /* closing punctuation */
                case STEF: printf("F:"); break; /* closing full stop */
            }
            printf("%s ", aawc->aaw[i]->aw[j]->w);
        }
        printf("(%uw)\n", aawc->aaw[i]->al); 
    }
    printf("\n"); 
    for(i=0;i<aawc->ppsz; i++) {
        printf("%i ", aawc->ppa[i]);
    }
    printf("\n"); 
}

void prtaawcdat3(aaw_c *aawc) /* print line and word details, but not the words themselves */
{
    int i, j;
    for(i=0;i<aawc->numl;++i) {
        for(j=0;j<aawc->aaw[i]->al;++j) {
            printf((j==aawc->aaw[i]->al-1)?"%s\n":"%s ", aawc->aaw[i]->aw[j]->w);
            if((aawc->aaw[i]->aw[j]->t == STEF) && (i!=aawc->numl-1))
                printf("\n");
        }
    }
}

aaw_c *processinpf(char *fname)
{
    /* declarations */
    FILE *fp=fopen(fname,"r");
    int i;
    size_t couc /*count chars per line */, couw=0 /* count words */, oldcouw;
    int c, oldc='\0', ooldc='\0' /* pcou=0 paragraph counter */;
    boole inword=0;
    boole linestart=1; /* want to catch number of stating spaces or tabs */
    unsigned lbuf=LBUF /* buffer for number of lines */, cbuf=CBUF /* char buffer for size of w_c's: reused for every word */;
    aaw_c *aawc=crea_aawc(lbuf); /* array of words per line */

    while( (c=fgetc(fp)) != EOF) {
        if( (c== '\n') | (c == ' ') | (c == '\t') ) {
            if( inword==1) { /* cue word-ending procedure */
                aawc->aaw[aawc->numl]->aw[couw]->w[couc++]='\0';
                aawc->aaw[aawc->numl]->aw[couw]->lp1=couc;
                SETCPTYPE(oldc, aawc->aaw[aawc->numl]->aw[couw]->t);
                norm_wc(aawc->aaw[aawc->numl]->aw+couw);
                couw++; /* verified: this has to be here */
            }
            if(c=='\n') { /* cue line-ending procedure */
                if(oldc=='\n') {
                    /* paragraph occurence */
                    CONDREALLOC(aawc->ppsz, aawc->ppb, CBUF, aawc->ppa, int);
                    aawc->ppa[aawc->ppsz++]=aawc->numl;
                    aawc->aaw[aawc->numl-1]->aw[oldcouw-1]->t=STEF;
                }
                if(couw != 0) {
                    if(aawc->numl ==lbuf-1) {
                        lbuf += LBUF;
                        aawc->aaw=realloc(aawc->aaw, lbuf*sizeof(aw_c*));
                        for(i=lbuf-LBUF; i<lbuf; ++i)
                            aawc->aaw[i]=crea_awc(WABUF);
                    }
                    aawc->aaw[aawc->numl]->al=couw;
                    norm_awc(aawc->aaw+aawc->numl);
                    aawc->numl++;
                    oldcouw=couw;
                    couw=0;
                    linestart=1;
                }
            } else if (linestart) /* must be a space or a tab */
                (c == ' ')? aawc->aaw[aawc->numl]->stsps++ : aawc->aaw[aawc->numl]->sttbs++;
            inword=0;
        } else if(inword==0) { /* a normal character opens word */
            if(couw ==aawc->aaw[aawc->numl]->ab-1) /* new word opening */
                reall_awc(aawc->aaw+aawc->numl, WABUF);
            couc=0;
            cbuf=CBUF;
            aawc->aaw[aawc->numl]->aw[couw]->w[couc++]=c;
            GETLCTYPE(c, aawc->aaw[aawc->numl]->aw[couw]->t); /* MACRO: the leading character gives a clue */
            inword=1;
            linestart=0;
        } else if(inword) { /* simply store */
            if(couc == cbuf-1)
                reall_wc(aawc->aaw[aawc->numl]->aw+couw, &cbuf);
            aawc->aaw[aawc->numl]->aw[couw]->w[couc++]=c;
            /* if word is a candidate for a NUM or PNI (i.e. via its first character), make sure it continues to obey rules: a MACRO */
            IWMODTYPEIF(c, aawc->aaw[aawc->numl]->aw[couw]->t);
        }
        ooldc=oldc;
        oldc=c;
    } /* end of big for statement */
    fclose(fp);

    /* so we know now that EOF has been reached ... this normally also means the final paragraph, so: */
    CONDREALLOC(aawc->ppsz, aawc->ppb, CBUF, aawc->ppa, int);
    aawc->ppa[aawc->ppsz++]=aawc->numl;
    aawc->aaw[aawc->numl-1]->aw[oldcouw-1]->t=STEF;
    printf("CC: %zu %zu %zu\n", aawc->numl, couw, oldcouw);

    /* normalization stage */
    for(i=aawc->numl; i<lbuf; ++i) {
        free_awc(aawc->aaw+i);
    }
    aawc->aaw=realloc(aawc->aaw, aawc->numl*sizeof(aw_c*));
    aawc->ppa=realloc(aawc->ppa, aawc->ppsz*sizeof(int));

    return aawc;
}

int main(int argc, char *argv[])
{
    /* argument accounting */
    if(argc!=2) {
        printf("pread, paragraph reader, another attempt.\n");
        printf("Error. Pls supply argument (name of text file).\n");
        exit(EXIT_FAILURE);
    }
#ifdef DBG2
    printf("typeszs: aaw_c: %zu aw_c: %zu w_c: %zu\n", sizeof(aaw_c), sizeof(aw_c), sizeof(w_c));
#endif

    aaw_c *aawc=processinpf(argv[1]);

    prtaawcdat2(aawc);
    // prtaawcdbg(aawc);
    printf("Numlines: %zu\n", aawc->numl); 
    printf("Numparas: %d\n", aawc->ppsz); 
    // prtaawapap(aawc);

    free_aawc(&aawc);

    return 0;
}
