#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "diatonic.h"

// populates s with data on every diatonic 2x2 pattern
void diagen(Pattern *s, short LIMIT) {
	// initialise boolean flags to 0
	for (short i = 0; i < LIMIT; i++) {
		s[i].nswapcheck = 0;
		s[i].oswapcheck = 0;
		s[i].jvfilter = 0;
		s[i].entryfilter = 0;
		s[i].seqfilter = 0;
		s[i].vpfilter = 0;
		s[i].mmfilter = 0;
	}
	// generate patterns
	patterngen(s);
	// generate derivative data for each pattern
	jvgen(s, LIMIT);
	entrygen(s, LIMIT);
	aliasgen(s, LIMIT);
	retrogen(s, LIMIT);
	nswapgen(s, LIMIT);
	oswapgen(s, LIMIT);
	seqgen(s, LIMIT);
}

// generate valid melodic models, then pass them to consonances to complete
void patterngen(Pattern *s) {
	// temporary array to store pattern
	short a[4];
	short i;

	// >>> MELODIC MODEL - FIRST INT <<<
	// move through melodic ints from down an 8ve to up an 8ve
	for (i = -7; i <= 7; i++) {
		// exclude leaps of sevenths
		if (i == -6 || i == 6)
			continue;
		a[0] = i;

		// >>> MELODIC MODEL - SECOND INT <<<
		// move through melodic ints from down an 8ve to up an 8ve
		for (short j = -7; j <= 7; j++) {
			// exclude leaps of sevenths
			if (j == -6 || j == 6)
				continue;
			// exclude unrecovered leaps larger than a 3rd (descending)
			if (a[0] < -2 && j < 0)
				j = 0;
			else if (a[0] < 0 && j < -2)
				j = -2;
			// exclude unrecovered leaps larger than a 3rd (ascending)
			if (a[0] > 2 && j > 0)
				break;
			else if (a[0] > 0 && j > 2)
				break;
			// cull duplicate models
			if (j > i)
				break;
			a[1] = j;
			// generate consonance pairs for each melodic model
			consonances(s, a, 4);
		}
	}
}

// generate consonance pairs for each melodic model
void consonances(Pattern *s, short *a, short len) {
	// count which increments with each valid pattern discovered
	static short count = 0;
	short n;

	// first cons is positive, second is never larger than 7
	for (short i = 0; i < (a[0] - a[1] + 7); i++) {
		// exclude dissonances
		if (i % 7 == 1 || i % 7 == 6)
			continue;
		// n stores second int values
		n = i + a[1] - a[0];
		if (abs(n) % 7 == 1 || abs(n) % 7 == 6)
			continue;
		// exclude negative compound ints except U7D7:7,-7
		if (abs(n) >= 7 && (i != 7 || n != -7))
			continue;
		// exclude parallel perfects
		if (a[0] != 0 && a[0] == a[1] && (i == 0 || i == 4))
			continue;
		// exclude counter-parallels
		if (a[0] - 7 == a[1] && (i == 7 || i == 11))
			continue;
		// if first cons is zero, second cons cannot be negative
		if (i == 0 && (i + a[1] - a[0]) < 0)
			continue;
		// first cons should always be larger than abs of negative 2nd cons
		if (i < abs(i + a[1] - a[0]))
			continue;
		// plug in consonances
		a[2] = i;
		a[3] = i + a[1] - a[0];

		s[count].value[0] = a[0];
		s[count].value[1] = a[1];
		s[count].value[2] = a[2];
		s[count].value[3] = a[3];
		count++;
	}
}

// generates JJv for each pattern in the struct
void jvgen(Pattern *s, short LIMIT) {
	short n;
	for (short i = 0; i < LIMIT; i++) {
		// calculate index
		n = -s[i].value[2] - s[i].value[3];
		// if index has a larger absolute value than 13, reduce
		while (n < -13)
			n += 7;
		// apply index value
		s[i].jv = n;
	}
}

// generates entrance intervals for each pattern in the struct
void entrygen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++) {
		// set first entry int
		s[i].entry[0] = s[i].value[0] + s[i].value[3];
		// set second entry int
		s[i].entry[1] = s[i].value[0] - s[i].value[2];
	}
}

// generates aliased versions of each pattern in the struct
void aliasgen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++) {
		s[i].alias[0] = s[i].value[1];
		s[i].alias[1] = s[i].value[0];
		s[i].alias[2] = s[i].value[3];
		s[i].alias[3] = s[i].value[2];
	}
}

// swaps to the opposite permutation of a pattern
void alias(short *a) {
	short swap;
	// swap model ints
	swap = a[0];
	a[0] = a[1];
	a[1] = swap;
	// swap tail ints
	swap = a[2];
	a[2] = a[3];
	a[3] = swap;
}

// generates retrogrades of each pattern in the struct
void retrogen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++) {
		s[i].retro[0] = -s[i].value[1];
		s[i].retro[1] = -s[i].value[0];
		s[i].retro[2] = s[i].value[2];
		s[i].retro[3] = s[i].value[3];
	}
}

// swaps the voice to which every other note belongs, saves if valid
void nswapgen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++) {
	// >>> CHECK NOTE SWAP WILL BE VALID <<<
	if (modelcheck(s[i].entry, 2) == 0)
		continue;
	// make sure nswap won't produce the original pattern
	if (s[i].value[3] == 0)
		continue;
	// >>> MODEL PASSED ALL CHECKS <<<

	// set nswapcheck boolean to 1
	s[i].nswapcheck = 1;
	// set melodic ints to entry ints of main pattern
	s[i].nswap[0] = s[i].entry[0];
	s[i].nswap[1] = s[i].entry[1];
	// tail is the same as main pattern tail, but second int flips sign
	s[i].nswap[2] = s[i].value[2];
	s[i].nswap[3] = -s[i].value[3];
	}
}

// checks if melodic model of a derived pattern is sensible (1) or not (0)
short modelcheck(short *a, short len) {
	// no melodic ints larger than 8ve
	if (abs(a[0]) > 7 || abs(a[1]) > 7)
		return 0;
	// no melodic 7ths
	if (abs(a[0]) == 6 || abs(a[1]) == 6)
		return 0;
	// no un-recovered leaps larger than a 3rd
	if (abs(a[0]) > 2 && abs(a[0] - a[1]) < abs(a[0]))
		return 0;
	if (abs(a[1]) > 2 && abs(a[1] - a[0]) < abs(a[1]))
		return 0;
	// pattern passed all checks
	return 1;
}

// octave swaps each pattern, saves if valid
void oswapgen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++) {
		// octave swaps the pattern
		s[i].oswap[1] = s[i].value[0] - 7;
		s[i].oswap[0] = s[i].value[1] + 7;
		s[i].oswap[3] = s[i].value[2] - 7;
		s[i].oswap[2] = s[i].value[3] + 7;

		cleanoswap(s, i);

		// if oswapped pattern is invalid or redundant, try salvaging
		if (modelcheck(s[i].oswap, 4) == 0 ||
			samecheck(s[i].oswap, 4, s[i].value, 4) == 1) {
			salvageswap(s, i);
		}

		if (s[i].oswap[0] < s[i].oswap[1])
			alias(s[i].oswap);

		// make sure oswapped pattern is valid and non-redundant again
		if (modelcheck(s[i].oswap, 4) == 0 ||
			samecheck(s[i].oswap, 4, s[i].value, 4) == 1)
			continue;
		s[i].oswapcheck = 1;
	}
}

// checks two patterns are not identical
short samecheck(short *a, short alen, short *b, short blen) {
	if (b[0] == a[0] && b[1] == a[1] && b[2] == a[2] && b[3] == a[3])
		return 1;
	return 0;
}

// make sure the consonances in an octave swapped pattern have a sensible range
void cleanoswap(Pattern *s, short i) {
	// if both consonances are compound ints, reduce until at least one isn't
	while (s[i].oswap[2] >= 7 && s[i].oswap[3] >= 7) {
		s[i].oswap[2] -= 7;
		s[i].oswap[3] -= 7;
	} 
	// if both consonances are negative, increase until at least one isn't
	while (s[i].oswap[2] < 0 && s[i].oswap[3] < 0) {
		s[i].oswap[2] += 7;
		s[i].oswap[3] += 7;
	}
}

// try to salvage an octave swapped pattern that failed its first check
void salvageswap(Pattern *s, short i) {
	// a small, b big, flip b and recalculate consonances and clean up
	if (abs(s[i].oswap[0]) < 3) {
		invertu(&s[i].oswap[1]);
		// recalculate consonances
		s[i].oswap[3] = s[i].oswap[2] + s[i].oswap[1] - s[i].oswap[0];
		cleanoswap(s, i);
	}
	// b small, a big, flip a and recalculate consonances and clean up
	if (abs(s[i].oswap[1]) < 3) {
		invertu(&s[i].oswap[0]);
		// recalculate consonances
		s[i].oswap[3] = s[i].oswap[2] + s[i].oswap[1] - s[i].oswap[0];
		cleanoswap(s, i);
	}
	// if a is larger than 7, reduce, recalculate consonances and clean
	if (abs(s[i].oswap[0]) > 7) {
		s[i].oswap[0] %= 7;
		// recalculate consonances
		s[i].oswap[3] = s[i].oswap[2] + s[i].oswap[1] - s[i].oswap[0];
		cleanoswap(s, i);
	}
	// if b is larger than 7, reduce, recalculate consonances and clean
	if (abs(s[i].oswap[1]) > 7) {
		s[i].oswap[1] %= 7;
		// recalculate consonances
		s[i].oswap[3] = s[i].oswap[2] + s[i].oswap[1] - s[i].oswap[0];
		cleanoswap(s, i);
	}
}

// generates the possible root sequences for every pattern and saves
void seqgen(Pattern *s, short LIMIT) {
	for (short i = 0; i < LIMIT; i++)
		// fine sequences for each pattern in the struct
		getseqs(s, i);
}

// find all possible root sequences a given pattern can support
void getseqs(Pattern *s, short n) {
	// >>> MECHANISM TO COMPUTE SUPPORTED ROOT MOTIONS <<<
	// storage for possible root positions WRT lower note of each consonance
	const short *possroot[6] = { 
    	(short[3]){ 0, -2, -4 },
    	(short[0]){},
    	(short[2]){ 0, -2 },
    	(short[1]){ -4 },
    	(short[1]){ 0 },
    	(short[2]){ -2, -4 },
	};
	// storage for the number of elements in possroot[n][]
	const short size[6] = { 3, 0, 2, 1, 1, 2 };

	// first and second half of root pattern
	short a, b;
	// first and second consonance from input pattern
	short x = s[n].value[2];
	short y = s[n].value[3];
	// for storing and comparing patterns to avoid duplication
	s[n].scount = 0;

	// reduce consonances to simple positive ints
	while (x < 0)
		x += 7;
	x %= 7;
	while (y < 0)
		y += 7;
	y %= 7;

	// work out all possible root sequences
	for (short i = 0; i < size[x]; i++) {
		for (short j = 0; j < size[y]; j++) {
			// set values for first and second half of root sequence
			a = s[n].value[0] + possroot[y][i] - possroot[x][j];
			b = s[n].value[0] + s[n].value[1] - a;
			// process intervals
			cleanseq(&a, &b);

			// check if model is already listed
			for (short k = s[n].scount; k > 0; k -= 2) {
				if (k >= 2 && a == s[n].seqs[k-2] && b == s[n].seqs[k-1])
					goto skip_pattern;
			}
			// pattern is unique: save intervals
			s[n].seqs[s[n].scount] = a;
			s[n].seqs[s[n].scount + 1] = b;
			// increase count ready for next pattern (or recall if complete)
			s[n].scount += 2;

			skip_pattern:
			;
		}
	}
	// sort sequences if not correctly ordered
	sortseqs(s[n].seqs, 10, s[n].scount);
}

// clean up the intervals in generated root sequence models
void cleanseq(short *a, short *b) {
	// make sure a and b are simple ints
	*a %= 7;
	*b %= 7;
	// algo to arrange output in normal order
	if (abs(*a) + abs(*b) >= 7) {
		// invert larger int at the 8ve
		if (abs(*a) > abs(*b)) {
			invert(a);
		} else {
			invert(b);
		}
	}
	// if both ints have same sign and combined magnitude > 4, flip the smaller
	if ((*a) * (*b) > 0 &&
		abs(*a) + abs(*b) > 4) {
		// invert first int at the 8ve
		if (abs(*a) > abs(*b))
			invert(a);
		// invert second int at the 8ve
		else
			invert(b);
	}
	// if either int is zero, the other should not exceed 3
	if ((*a == 0 || *b == 0) &&
		(abs(*a) > 3 || abs(*b) > 3)) {
		// invert first int at the 8ve
		if (*a != 0)
			invert(a);
		// invert second int at the 8ve
		else
			invert(b);
	}
	// if either int is larger than 2 and signs the same, flip larger
	if ((abs(*a) > 2 || abs(*b) > 2) &&
		((*a) * (*b) > 0)) {
		if (abs(*a) > abs(*b))
			invert(a);
		else
			invert(b);
	}
	// no int should be larger than 4 in magnitude
	if (abs(*a) > 4)
		invert(a);
	if (abs(*b) > 4)
		invert(b);
	// make sure it's in normal order (again)
	if (*a < *b) {
		short swap = *a;
		*a = *b;
		*b = swap;
	}
}

// inverts an interval at the octave, unisons become -7
void invert(short *n) {
	*n %= 7;
	if (*n >= 0)
		*n -= 7;
	else
		*n += 7;
}

// inverts an interval at the octave, preserves unisons
void invertu(short *n) {
	*n %= 7;
	if (*n > 0)
		*n -= 7;
	else if (*n < 0)
		*n += 7;
}

// bubble sorts sequences stored in array string
void sortseqs(short *a, short len, short count) {
	short swap;
	
	// check if int 1 of each seq is larger than the seq before, swaps if true
	for (short i = count - 2; i >= 2; i -= 2) {
		if (a[i] > a[i-2]) {
			swap = a[i];
			a[i] = a[i-2];
			a[i-2] = swap;
			swap = a[i+1];
			a[i+1] = a[i-1];
			a[i-1] = swap;
		}
	}
	// checks if the patterns are sorted, repeats process if not
	for (short i = count - 2; i >= 2; i -= 2)
		if (a[i] > a[i-2])
			sortseqs(a, 4, count);	
}

// convert s[].seqs[] short array to char string
char* stringseqs(short *a, short len, short count) {
	static char str[4];
	short strcount;

	for (short i = 0; i <= count - 2; i += 2) {
		strcount = 0;
		// FIRST INT CASES
		// first int is greater than zero
		if (a[i] > 0) {
			str[0] = 'U';
			str[1] = a[i] + '0';
			strcount += 2;
		// first int is negative
		} else if (a[i] < 0) {
			str[0] = 'D';
			str[1] = abs(a[i]) + '0';
			strcount += 2;
		// first int is zero
		} else {
			str[0] = 'S';
			strcount++;
		}
		// SECOND INT CASES
		// second int is greater than zero
		if (a[i+1] > 0)
		{
			str[strcount] = 'U';
			str[strcount + 1] = a[i+1] + '0';
			strcount += 2;
		// second int is less than zero
		} else if (a[i+1] < 0) {
			str[strcount] = 'D';
			str[strcount + 1] = abs(a[i+1]) + '0';
			strcount += 2;
		// second int is zero
		} else {
			str[strcount] = 'S';
			strcount ++;
		}
		str[strcount] = '\0';
		printf("%s\t", str);
	}
	return str;
}

// converts from numerical to hexcode string form, eg. 2 -1 5 2 -> U2D1:5,2
char* hexcode(short *a, short len) {
	static char str[12];
	short length = 0;

	// compute first two ints
	hexhead(a, 4, str, 12, &length);
	str[length] = ':';
	++length;

	// THIRD INT CASES
	// is a '-' sign needed?
	if (a[2] < 0) {
		str[length] = '-';
		length++;
	}
	// third int has one digit
	if (abs(a[2]) < 10) {
		str[length] = abs(a[2]) + '0';
		length++;
	// third int has two digits
	} else {
		str[length] = abs(a[2]) / 10 + '0';
		str[length + 1] = abs(a[2]) % 10 + '0';
		length += 2;
	}
	str[length] = ',';
	length++;
	// FOURTH INT CASES
	// is a '-' sign needed?
	if (a[3] < 0) {
		str[length] = '-';
		length++;
	}
	// fourth int has one digit
	if (abs(a[3]) < 10) {
		str[length] = abs(a[3]) + '0';
		length++;
	// fourth int has two digits
	} else {
		str[length] = abs(a[3]) / 10 + '0';
		str[length + 1] = abs(a[3]) % 10 + '0';
		length += 2;
	}
	str[length] = '\0';
	return str;
}

// converts from numerical to LaTeX-formatted hexcode string form
char* hexcodef(short *a, short len) {
	static char str[24];
	static char strf[11] = {'$', '-', '\\', 't', 'e', 'x', 't', 'r', 'm', '{'};
	short length = 0;

	// compute first two ints
	hexhead(a, 4, str, 12, &length);
	str[length] = ':';
	++length;

	// THIRD INT CASES
	// third int is positive
	if (a[2] >= 0) {
		// third int has one digit
		if (a[2] < 10) {
			str[length] = a[2] + '0';
			length++;
		// third int has two digits
		} else {
			str[length] = a[2] / 10 + '0';
			str[length + 1] = a[2] % 10 + '0';
			length += 2;
		}
	// third int is negative
	} else {
		// insert LaTeX math-mode chars
		for (int i = 0; i < 10; i++) {
			str[length] = strf[i];
			length++;
		}
		// third int has one digit
		if (abs(a[2]) < 10) {
			str[length] = abs(a[2]) + '0';
			length++;
		// third int has two digits
		} else {
			str[length] = abs(a[2]) / 10 + '0';
			str[length + 1] = abs(a[2]) % 10 + '0';
			length += 2;
		}
		// close LaTeX math-mode function
		str[length] = '}';
		str[length + 1] = '$';
		length += 2;
	}
	str[length] = ',';
	length++;

	// FOURTH INT CASES
	// fourth int is positive
	if (a[3] >= 0) {
		// fourth int has one digit
		if (a[3] < 10) {
			str[length] = a[3] + '0';
			length++;
		// fourth int has two digits
		} else {
			str[length] = a[3] / 10 + '0';
			str[length + 1] = a[3] % 10 + '0';
			length += 2;
		}
	// fourth int is negative
	} else {
		// insert LaTeX math-mode chars
		for (int i = 0; i < 10; i++) {
			str[length] = strf[i];
			length++;
		}
		// fourth int has one digit
		if (abs(a[3]) < 10) {
			str[length] = abs(a[3]) + '0';
			length++;
		// fourth int has two digits
		} else {
			str[length] = abs(a[3]) / 10 + '0';
			str[length + 1] = abs(a[3]) % 10 + '0';
			length += 2;
		}
		// close LaTeX math-mode function
		str[length] = '}';
		str[length + 1] = '$';
		length += 2;
	}
	str[length] = '\0';

	return str;
}

// converts melodic model from numerical to hexcode string form
void hexhead(short *a, short lenone, char *str, short lentwo, short *length) {
	// FIRST INT CASES
	// first int is greater than zero
	if (a[0] > 0) {
		str[*length] = 'U';
		str[*length + 1] = a[0] + '0';
		*length += 2;
	// first int is less than zero
	} else if (a[0] < 0) {
		str[*length] = 'D';
		str[*length + 1] = abs(a[0]) + '0';
		*length += 2;
	// first int is zero
	} else {
		str[*length] = 'S';
		++*length;
	}
	// SECOND INT CASES
	// second int is greater than zero
	if (a[1] > 0) {
		str[*length] = 'U';
		str[*length + 1] = a[1] + '0';
		*length += 2;
	// second int is less than zero
	} else if (a[1] < 0) {
		str[*length] = 'D';
		str[*length + 1] = abs(a[1]) + '0';
		*length += 2;
	// second int is zero
	} else {
		str[*length] = 'S';
		++*length;
	}
}

// calculates the maximum distance spanned by three iterations of a pattern
short getrange(Pattern *s, short n) {
	// starting value for range is the initial distance between voices
	short range = s[n].value[2];
	// find and store highest note in top voice
	short top = highestnote(s, n);
	// bottom voice's lowest note (relative to starting pos) will be stored here
	short bottom = 0;
	// storage for current bottom notes
	short b = 0;
	// find lowest note in bottom voice
	for (short i = 0; i < 2; i++) {
	if (bottom > (b += s[n].value[0]))
		bottom = b;
	if (bottom > (b += s[n].value[1]))
		bottom = b;
	}
	if (bottom > (b += s[n].value[0]))
		bottom = b;
	range += top - bottom;

	return range;
}

// finds the highest number of steps reached above the start
short highestnote(Pattern *s, short n) {
	// top voice's highest note (relative to starting pos) will be stored here
	short top = 0;
	// storage for current top note
	short a = 0;

	// find highest note in upper voice
	// least dumb way to do A->B->A->B->A? vOv
	for (short i = 0; i < 2; i++) {
	if (top < (a += s[n].value[1]))
		top = a;
	if (top < (a += s[n].value[0]))
		top = a;
	}
	if (top < (a += s[n].value[1]))
		top = a;

	return top;
}

// finds the starting position of the upper voice with respect to B4
short startpos(Pattern *s, short n) {
	return (getrange(s, n) / 2) - highestnote(s, n) + 13;
}

// generates lilypond note string for 3 iterations of the pattern
char* lilygen(Pattern *s, short n) {
	char head[] = "<< { \\rednotes ";
	char middle[] = "} \\\\ { \\bluenotes ";
	char tail[] = "} >>\n";
	static char str[100];
	short i;
	short note = startpos(s, n);

	// add head to string	
	for (i = 0; head[i] != '\0'; i++)
		str[i] = head[i];
	
	// calculate and add top voice notes to string
	str[i] = (note + 2) % 7 + 'a';
	i++;
	for (short j = note / 7; j > 0; j--) {
		str[i] = '\'';
		i++;
	}
	str[i] = '2';
	i++;
	str[i] = ' ';
	i++;
	for (short j = 0; j < 2; j++) {
		note += s[n].value[1];
		nextnote(s, str, &note, &i);
		note += s[n].value[0];
		nextnote(s, str, &note, &i);
	}
	note += s[n].value[1];
	nextnote(s, str, &note, &i);

	// adds middle to string
	for (short j = 0; middle[j] != '\0'; j++) {
		str[i] = middle[j];
		i++;
	}

	// calculate and add bottom voice notes to string
	note = startpos(s, n) - s[n].value[2];
	str[i] = (note + 2) % 7 + 'a';
	i++;
	for (short j = note / 7; j > 0; j--) {
		str[i] = '\'';
		i++;
	}
	str[i] = '2';
	i++;
	str[i] = ' ';
	i++;
	for (short j = 0; j < 2; j++) {
		note += s[n].value[0];
		nextnote(s, str, &note, &i);
		note += s[n].value[1];
		nextnote(s, str, &note, &i);
	}
	note += s[n].value[0];
	nextnote(s, str, &note, &i);

	// adds tail to string
	for (short j = 0; tail[j] != '\0'; j++) {
		str[i] = tail[j];
		i++;
	}


	str[i] = '\0';
	return str;
}

// calculates how a note needs to be printed and adds it to the string
void nextnote(Pattern *s, char *str, short *note, short *i) {
	str[*i] = (*note + 2) % 7 + 'a';
		++*i;
	for (short j = *note / 7; j > 0; j--) {
		str[*i] = '\'';
		++*i;
	}
	str[*i] = ' ';
	++*i;
}
