void diagen(Pattern *s, short LIMIT);
void patterngen(Pattern *s);
void consonances(Pattern *s, short *a, short len);
void jvgen(Pattern *s, short LIMIT);
void entrygen(Pattern *s, short LIMIT);
void aliasgen(Pattern *s, short LIMIT);
void alias(short *a);
void retrogen(Pattern *s, short LIMIT);
void nswapgen(Pattern *s, short LIMIT);
void oswapgen(Pattern *s, short LIMIT);
short modelcheck(short *a, short len);
short samecheck(short *a, short alen, short *b, short blen);
void cleanoswap(Pattern *s, short i);
void salvageswap(Pattern *s, short i);
void seqgen(Pattern *s, short LIMIT);
void getseqs(Pattern *s, short n);
void cleanseq(short *firsthalf, short *secondhalf);
void invert(short *n);
void invertu(short *n);
void sortseqs(short *a, short len, short count);
char* stringseqs(short *a, short len, short count);
char* hexcode(short *a, short len);
char* hexcodef(short *a, short len);
void hexhead(short *a, short lenone, char *str, short lentwo, short *length);
short getrange(Pattern *s, short n);
short highestnote(Pattern *s, short n);
short startpos(Pattern *s, short n);
char* lilygen(Pattern *s, short n);
void nextnote(Pattern *s, char *str, short *note, short *i);