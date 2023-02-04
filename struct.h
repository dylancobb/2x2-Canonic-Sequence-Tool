// struct to hold all the pattern data and filter booleans
typedef struct {
	short value[4];
	short jv;
	short entry[2];
	short alias[4];
	short retro[4];
	short nswap[4];
	short nswapcheck;
	short oswap[4];
	short oswapcheck;
	short seqs[10];
	short scount;
	short jvfilter;
	short entryfilter;
	short seqfilter;
	short vpfilter;
	short mmfilter;
} Pattern;
