#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "diatonic.h"

void titleprint(void);
void menu(Pattern *s, short *filtercheck, short LIMIT, char *option);
void printshort(Pattern *s, short *filtercheck, short LIMIT, char *option);
void printlong(Pattern *s, short *filtercheck, short LIMIT, char *option);
short fcheck(Pattern *s, short *filtercheck, short i);
void filter(Pattern *s, short *filtercheck, short LIMIT, char *option);
void resetf(Pattern *s, short *filtercheck, short LIMIT, char *option);
void howmany(Pattern *s, short *filtercheck, short LIMIT, char *option);
void filterjv(Pattern *s, short *filtercheck, short LIMIT, char *option);
void filterentry(Pattern *s, short *filtercheck, short LIMIT, char *option);
void filterseq(Pattern *s, short *filtercheck, short LIMIT, char *option);
void filtervp(Pattern *s, short *filtercheck, short LIMIT, char *option);
void filtermm(Pattern *s, short *filtercheck, short LIMIT, char *option);
void textemplate(Pattern *s, short LIMIT);
char* filecode(short *a, short len);
char* titlecode(short *a, short len);
char* headstr(short *a, short len);

int main(void) {
	// const can be set to the total number of Pattern in the collection when known
	const short LIMIT = 256;
	// declare the main array "s" that will store all pattern data
	Pattern s[LIMIT];
	// array to track which Filter are in use
	// 0 = jv, 1 = entry, 2 = seq, 3 = voice pair
	short filtercheck[5] = {0, 0, 0, 0, 0};

	titleprint();

	// BEGIN USER ROUTINE
	printf("Which type of 2x2 pattern do you wish to generate?\n\n");
	printf("1 = Diatonic\t\t2 = Chromatic\n\n");
	char option;
	do {
		printf("Enter: ");
		scanf(" %c", &option);
	}
	while ((option > '2' || option < '1') && option != 'q');
	// pass to correct function based on input (or quits if 'q')
	if (option == '1') {
		diagen(s, LIMIT);
		printf("\n%hi Pattern generated. ", LIMIT);
		printf("What would you like do do?\n\n");
		menu(s, filtercheck, LIMIT, &option);
	}
	// [TODO] build chromatic pattern gen

	return 0;
}

// old-school printed title
void titleprint(void) {
printf("\n    -  -      _  _\n");
printf("  \\   |\\  \\  | || |_____ __   ");
printf(">>>>> CANONIC PATTERN GENERATOR <<<<<\n");
printf("|\\ \\  | )  | | __ / -_) \\ /\n");
printf("| \\_| |    | |_||_\\___/_\\_\\      ");
printf("enter \"q\" at any time to quit)\n");
printf(" \\   @    /   Music Theory\n");
printf("    _  _\t\t\t more info at hexmusictheory.com\n\n");
}

// menu where users can choose to print Pattern or filter more
void menu(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	// print user options
	printf("1 = Print\t\t4 = Reset Filters\n");
	printf("2 = Print (Verbose)\t5 = Export All (.tex)\n");
	printf("3 = Add Filter\n\n");
	// take user input
	do {
		printf("Enter: ");
		scanf(" %c", option);
	}
	// pass to correct function based on input (or quits if 'q')
	while ((*option > '5' || *option < '1') && *option != 'q');
	switch (*option)
	{
		case '1':
			printshort(s, filtercheck, LIMIT, option);
			break;
		case '2':
			printlong(s, filtercheck, LIMIT, option);
			break;
		case '3':
			filter(s, filtercheck, LIMIT, option);
			break;
		case '4':
			resetf(s, filtercheck, LIMIT, option);
			break;
		case '5':
			textemplate(s, LIMIT);
			break;
		case 'q': default:
			return;
	}
}

// prints every pattern that passes filtering
void printshort(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	printf("\n================PATTERNS================\n\n");
	for (short i = 0; i < LIMIT; i++) {
		// check if pattern passes filters
		if(fcheck(s, filtercheck, i) == 0)
			continue;

		// print Pattern that pass filtering
		printf("%s\n", hexcode(s[i].value, 4));
	}
	printf("\n===============END OF LIST===============\n\n");
	// return user to the main menu
	menu(s, filtercheck, LIMIT, option);
}

// prints every pattern that passes filtering and all its derivative data
void printlong(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	printf("\n================PATTERNS================\n\n");
	for (short i = 0; i < LIMIT; i++)
	{
		// check if pattern passes filters
		if(fcheck(s, filtercheck, i) == 0)
			continue;

		// print Pattern that pass filtering
		printf("%s\n", hexcode(s[i].value, 4));
		printf("\tElaborates at Jv = %hi\n", s[i].jv);
		printf("\tEntry ints: %hi, %hi\n", s[i].entry[0], s[i].entry[1]);
		if (samecheck(s[i].value, 4, s[i].alias, 4) != 1)
			printf("\tAlias: %s\n", hexcode(s[i].alias, 4));
		if (s[i].value[0] != -s[i].value[1])
			printf("\tRetrograde: %s\n", hexcode(s[i].retro, 4));
		if (s[i].nswapcheck == 1)
			printf("\tNote Swap: %s\n", hexcode(s[i].nswap, 4));
		if (s[i].oswapcheck == 1)
			printf("\t8ve Swap: %s\n", hexcode(s[i].oswap, 4));
		printf("\tRoot Sequences: ");
		stringseqs(s[i].seqs, 10, s[i].scount);
		printf("\n");
	}
	printf("\n===============END OF LIST===============\n\n");
	// returns user to main menu
	menu(s, filtercheck, LIMIT, option);
}

// check if pattern passes filters
short fcheck(Pattern *s, short *filtercheck, short i) {
	if(filtercheck[0] == 1 && s[i].jvfilter == 0)
		return 0;
	if(filtercheck[1] == 1 && s[i].entryfilter == 0)
		return 0;
	if(filtercheck[2] == 1 && s[i].seqfilter == 0)
		return 0;
	if(filtercheck[3] == 1 && s[i].vpfilter == 0)
		return 0;
	if(filtercheck[4] == 1 && s[i].mmfilter == 0)
		return 0;
// pattern passed all checks
return 1;
}

// asks what filter the user wants to add and passes to the correct function
void filter(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	printf("\nFilter type:\n\n");
	printf("1 = Jv\t\t\t4 = Voice Pair\n");
	printf("2 = Entry Int\t\t5 = Melodic Motion\n");
	printf("3 = Root Sequence\n\n");
	// request user input
	do {
		printf("Enter: ");
		scanf(" %c", option);
	}
	while ((*option > '5' || *option < '1') && *option != 'q');
	// pass to correct function based on input (or quits if 'q')
	if (*option == '1') {
		filterjv(s, filtercheck, LIMIT, option);
	}
	else if (*option == '2') {
		filterentry(s, filtercheck, LIMIT, option);
	}
	else if (*option == '3') {
		filterseq(s, filtercheck, LIMIT, option);
	}
	else if (*option == '4') {
		filtervp(s, filtercheck, LIMIT, option);
	}
	else if (*option == '5') {
		filtermm(s, filtercheck, LIMIT, option);
	}
	// not explicit, but entering q will do nothing, thereby ending the program
}

// removes all applied filters
void resetf(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	filtercheck[0] = 0;
	filtercheck[1] = 0;
	filtercheck[2] = 0;
	filtercheck[3] = 0;
	filtercheck[4] = 0;
	for (short i = 0; i < LIMIT; i++) {
		s[i].jvfilter = 0;
		s[i].entryfilter = 0;
		s[i].seqfilter = 0;
		s[i].vpfilter = 0;
		s[i].mmfilter = 0;
	}
	printf("\n%hi patterns. What would you like to do?\n\n", LIMIT);
	menu(s, filtercheck, LIMIT, option);
}

// counts how many patterns meet the current filter criteria and prints answer
void howmany(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	short count = 0;

	for (short i = 0; i < LIMIT; i++) {
		// check if pattern passes filters
		if(fcheck(s, filtercheck, i) == 0)
			continue;
		count++;
	}
	printf("\n%hi Pattern found. ", count);
}

// lets the user look for patterns by elaboration JJv
void filterjv(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	short jv;
	char str[5];

	do {
		// prompts the user for Jv
		printf("Enter Jv: ");
		scanf("%s", str);
		// listens for quit command
		if (str[0] == 'q')
			return;
		// convert to integer
		jv = atoi(str);
	}
	while ((jv > 0 || jv < -13));
	// filter for desired patterns
	filtercheck[0] = 1;
	for (short i = 0; i < LIMIT; i++) {
		if (s[i].jv == jv)
			s[i].jvfilter = 1; 
	}
	// return user to main menu
	howmany(s, filtercheck, LIMIT, option);
	printf("What would you like to do?\n\n");
	menu(s, filtercheck, LIMIT, option);
}

// lets the user look for patterns by entrance intervals
void filterentry(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	short entry;
	char str[5];

	do
	{
		// prompts the user for input
		printf("Enter Int: ");
		scanf("%s", str);
		// listens for quit command
		if (str[0] == 'q')
			return;
		// convert to integer
		entry = atoi(str);
	}
	while ((entry > 12 || entry < -12));
	// filter for desired patterns
	filtercheck[1] = 1;
	for (short i = 0; i < LIMIT; i++) {
		if (s[i].entry[0] == entry || s[i].entry[1] == entry)
			s[i].entryfilter = 1; 
	}
	// return user to main menu
	howmany(s, filtercheck, LIMIT, option);
	printf("What would you like to do?\n\n");
	menu(s, filtercheck, LIMIT, option);
}

// lets the user look for patterns by specific supported harmonic sequences
void filterseq(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	short first, second;
	char str[5];

	do {
		// prompts the user for input
		printf("Enter First Int: ");
		scanf("%s", str);
		// listens for quit command
		if (str[0] == 'q')
			return;
		// convert to integer
		first = atoi(str);
	}
	while ((first > 7 || first < -7) && first != 'q');
	do {
		// prompts the user for input
		printf("Enter Second Int: ");
		scanf("%s", str);
		// listens for quit command
		if (str[0] == 'q')
			return;
		// convert to integer
		second = atoi(str);
	}
	while ((second > 7 || second < -7) && second != 'q');
	// prepare entered sequence for comparison
	cleanseq(&first, &second);
	// filter for desired patterns
	filtercheck[2] = 1;
	for (short i = 0; i < LIMIT; i++) {
		for (short j = s[i].scount; j > 0; j -= 2)
			if (j >= 2 && first == s[i].seqs[j-2] && second == s[i].seqs[j-1])
				s[i].seqfilter = 1;
	}
	// return user to main menu
	howmany(s, filtercheck, LIMIT, option);
	printf("What would you like to do?\n\n");
	menu(s, filtercheck, LIMIT, option);
}

// lets the user filter patterns based on voice pair applicability
void filtervp(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	printf("Voice pair suitability:\n\n");
	printf("1 = All Pairs\t\t2 = Non-Outer Pairs\n");
	printf("3 = Upper Pairs Only\n\n");
	// prompts the user for input
	do {
		printf("Enter: ");
		scanf(" %c", option);
	}
	while ((*option > '3' || *option < '1') && *option != 'q');
	// turn filter boolean flag on
	filtercheck[3] = 1;
	// filter for patterns that can be deployed in the outer voice pair
	if (*option == '1') {
		for (short i = 0; i < LIMIT; i++) {
			s[i].vpfilter = 1;
			// fail if either consonance is a fourth
			if (abs(s[i].value[2] % 7) == 3 || abs(s[i].value[3] % 7) == 3)
				s[i].vpfilter = 0;
			// fail if direct 5ths or 8ves
			if ((abs(s[i].value[0] + s[i].value[1]) > abs(s[i].value[0])) &&
			(abs(s[i].value[2] % 7) == 4 || abs(s[i].value[2] % 7) == 0 ||
			abs(s[i].value[3] % 7) == 4 || abs(s[i].value[3] % 7) == 0))
				s[i].vpfilter = 0;
			// fail if no imperfect consonances present
			if ((abs(s[i].value[2] % 7) == 4 || abs(s[i].value[2] % 7) == 0) &&
			(abs(s[i].value[3] % 7) == 4 || abs(s[i].value[3] % 7) == 0))
				s[i].vpfilter = 0;
		}
	// filter for patterns that can be deployed in all non-outer pairs
	} else if (*option == '2') {
		for (short i = 0; i < LIMIT; i++) {
			s[i].vpfilter = 1;
			// fail if either consonance is a fourth
			if (abs(s[i].value[2] % 7) == 3 || abs(s[i].value[3] % 7) == 3)
				s[i].vpfilter = 0;
		}
	// filter for patterns which are only applicable in upper voice pairs
	} else if (*option == '3') {
		for (short i = 0; i < LIMIT; i++) {
			s[i].vpfilter = 0;
			// passes if either consonance is a fourth
			if (abs(s[i].value[2] % 7) == 3 || abs(s[i].value[3] % 7) == 3)
				s[i].vpfilter = 1;
		}
	}
	// return user to main menu
	if (*option != 'q') {
	howmany(s, filtercheck, LIMIT, option);
	printf("What would you like to do?\n\n");
	menu(s, filtercheck, LIMIT, option);
	}
	// not explicit, but entering q will do nothing, thereby ending the program
}

// lets the user filter patterns for specific melodic motions
void filtermm(Pattern *s, short *filtercheck, short LIMIT, char *option) {
	short mint;
	char str[5];
	
	// prompts the user for input
	do {
		printf("Enter Int: ");
		scanf("%s", str);
		// listens for quit command
		if (str[0] == 'q')
			return;
		// convert to integer
		mint = atoi(str);
	}
	while ((mint > 7 || mint < -7) && mint != 'q');
	// apply filters
	filtercheck[4] = 1;
	for (short i = 0; i < LIMIT; i++)
		if (s[i].value[0] == mint || s[i].value[1] == mint)
			s[i].mmfilter = 1;
	// return user to main menu
	howmany(s, filtercheck, LIMIT, option);
	printf("What would you like to do?\n\n");
	menu(s, filtercheck, LIMIT, option);
}

// creates LaTeX files for each pattern for the book
void textemplate(Pattern *s, short LIMIT)
{
	for (short i = 0; i < LIMIT; i++) {
		// creates a file for the pattern and opens to write
		char* filename = filecode(s[i].value, 4);
		FILE *f = fopen(filename, "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}

		// LaTeX formatting pre-amble
		fprintf(f, "\\begin{adjustwidth*}{0in}{-1.127in}\n");
		fprintf(f, "\\subsection{%s}\n", titlecode(s[i].value, 4));
		// reference label for other patterns to call
		fprintf(f, "\\label{%s}\n", hexcode(s[i].value, 4));

		// INDEXING COMMANDS
		// index by pattern name
		fprintf(f, "\\index[patterns]{%s!%s}\n",
				headstr(s[i].value, 4), hexcodef(s[i].value, 4));
		// index by aliased name (if non-redundant)
		if (samecheck(s[i].value, 4, s[i].alias, 4) == 0)
			fprintf(f, "\\index[patterns]{%s!%s}\n",
					headstr(s[i].alias, 4), hexcodef(s[i].alias, 4));
		// index by Jv of elaboration
		fprintf(f, "\\index[jvindex]{$\\bm{Jv=%hi}$!%s}\n",
				s[i].jv, hexcodef(s[i].value, 4));

		// decides which arrow to use to display the entry intervals
		char *firstarrow = "";
		char *secondarrow = "";
		// first arrow
		if (s[i].entry[0] > 0)
			firstarrow = "\\nearrow";
		else if (s[i].entry[0] < 0)
			firstarrow = "\\searrow";
		else
			firstarrow = "\\rightarrow";
		// second arrow
		if (s[i].entry[1] > 0)
			secondarrow = "\\nearrow";
		else if (s[i].entry[1] < 0)
			secondarrow = "\\searrow";
		else
			secondarrow = "\\rightarrow";
		// index by entrance intervals
		fprintf(f, "\\index[entryint]{$%s %d$!%s}\n",
				firstarrow, abs(s[i].entry[0]), hexcodef(s[i].value, 4));
		fprintf(f, "\\index[entryint]{$%s %d$!%s}\n",
				secondarrow, abs(s[i].entry[1]), hexcodef(s[i].value, 4));

		// index by root sequence(s)
		switch (s[i].scount/2) {
			case 3:
				fprintf(f, "\\index[sequencetype]{%s!%s}\n",
					headstr((s[i].seqs + 4), 2), hexcodef(s[i].value, 4));
			case 2:
				fprintf(f, "\\index[sequencetype]{%s!%s}\n",
					headstr((s[i].seqs + 2), 2), hexcodef(s[i].value, 4));
			case 1:
				fprintf(f, "\\index[sequencetype]{%s!%s}\n",
					headstr(s[i].seqs, 2), hexcodef(s[i].value, 4));
		}

		// prints a dynamically generated musical example for the pattern
		fprintf(f, "\\begin{bigstaff}\n");
		fprintf(f, "%s", lilygen(s, 4));
		fprintf(f, "\\end{bigstaff}\\vspace*{1em}\n");

		// opens a table for the printed pattern data
		fprintf(f, "\\begin{center}\n");
		fprintf(f, "\\begin{tabular*}{\\textwidth}{l @{");
		fprintf(f, "\\extracolsep{\\fill}} l}\n");

		// establishes whether pattern elaborates at a 1Jv or 2Jv
		short jvtype;
		switch (s[i].jv) {
			case 0: case -3: case -4: case -7: case -10: case -11:
			jvtype = 1;
			break;
			default:
			jvtype = 2;
			break;
		}

		// prints formatted data about the pattern
		fprintf(f, "\\textbf{Elaborates at:} $^%hiJv=%hi$&\n", jvtype, s[i].jv);
		fprintf(f, "\\textbf{Alias:} ");
		if (s[i].value[0] != s[i].value[1])
			fprintf(f, "%s\\\\\n", hexcodef(s[i].alias, 4));
		else
			fprintf(f, "N/A\\\\\n");
		fprintf(f, "\\textbf{Entry intervals:} $%s %d, %s %d$&\n",
				firstarrow, abs(s[i].entry[0]), secondarrow, abs(s[i].entry[1]));
		fprintf(f, "\\textbf{Retrograde:} ");
		if (s[i].value[0] != -s[i].value[1])
			fprintf(f, "%s (page \\pageref{%s})\\\\\n",
					hexcodef(s[i].retro, 4), hexcode(s[i].retro, 4));
		else
			fprintf(f, "N/A\\\\\n");
		if (s[i].nswapcheck == 1)
			fprintf(f, "\\textbf{Note Swap:} %s (page \\pageref{%s})",
					hexcodef(s[i].nswap, 4), hexcode(s[i].nswap, 4));
		else
			fprintf(f, "\\textbf{Note Swap:} N/A");
		if (s[i].oswapcheck == 1)
			fprintf(f, "&\n\\textbf{8ve Swap:} %s (page \\pageref{%s})",
					hexcodef(s[i].oswap, 4), hexcode(s[i].oswap, 4));
		else
			fprintf(f, "\\textbf{8ve Swap:} N/A");
		fprintf(f, "\\\\\n");
		// ends table
		fprintf(f, "\\end{tabular*}\\vspace*{1em}\n");

		// starts new one-row table for printing the root sequences to
		fprintf(f, "\\begin{tabular*}{\\textwidth}{l @{");
		fprintf(f, "\\extracolsep{\\fill}} l}\n");
		// print root sequence data
		switch (s[i].scount/2) {
			case 3:
				fprintf(f, "\\textbf{Implied root motion:} %s, ",
					headstr((s[i].seqs), 2));
				fprintf(f, "%s, ",
					headstr((s[i].seqs + 2), 2));
				fprintf(f, "or %s\n",
					headstr((s[i].seqs + 4), 2));
				break;
			case 2:
				fprintf(f, "\\textbf{Implied root motion:} %s, ",
					headstr((s[i].seqs), 2));
				fprintf(f, "or %s\n",
					headstr((s[i].seqs + 2), 2));
				break;
			case 1:
				fprintf(f, "\\textbf{Implied root motion:} %s\n",
					headstr(s[i].seqs, 2));
				break;
		}
		// end table
		fprintf(f, "\\end{tabular*}\n");
		fprintf(f, "\\end{center}\\vspace*{1em}\n");

		// print info boxes if the pattern incurs any noteworthy restrictions
		if (abs(s[i].value[2] % 7) == 3 || abs(s[i].value[3] % 7) == 3)
		{
			fprintf(f, "\\bangbox{this pattern is only suitable for ");
			fprintf(f, "deployment in upper voice pairs.}\n");
		} else if (s[i].value[0] * s[i].value[1] > 0 &&
				(s[i].value[2] % 7 == 0 || s[i].value[2] % 7 == 4 ||
				s[i].value[3] % 7 == 0 || s[i].value[3] % 7 == 4)) {
			fprintf(f, "\\bangbox{direct motion to perfect ints make this ");
			fprintf(f, "pattern weak in the outer voice pair.}\n");
		} else if ((s[i].value[2] % 7 == 0 || s[i].value[2] % 7 == 4) &&
				(s[i].value[3] % 7 == 0 || s[i].value[3] % 7 == 4)) {
			fprintf(f, "\\bangbox{alternating perfect ints make this ");
			fprintf(f, "pattern weak in the outer voice pair.}\n");
		}

		// finish the LaTeX formatting block for the pattern
		fprintf(f, "\\end{adjustwidth*}\\vspace*{0.5em}");

		// all done!
		fclose(f);
		printf("%s created. (%d/%hi)\n", filename, i + 1, LIMIT);
	}
}

// creates an individual filename for textemplate()
char* filecode(short *a, short len) {
	static char str[20];
	short length = 0;

	// compute first two ints
	hexhead(a, 4, str, 12, &length);
	str[length] = ';';
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
	str[length] = '.';
	str[length + 1] = 't';
	str[length + 2] = 'e';
	str[length + 3] = 'x';
	str[length + 4] = '\0';
	return str;
}

// creates title string with \bm{ } (vs. hexcodec() which doesn't use \bm)
char* titlecode(short *a, short len) {
	static char str[30];
	static char strf[] = "$\\bm{-\\textrm{";
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
		for (int i = 0; strf[i] != '\0'; i++) {
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
		for (int i = 0; strf[i] != '\0'; i++) {
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
		str[length + 1] = '}';
		str[length + 2] = '$';
		length += 3;
	}
	str[length] = '\0';

	return str;
}

// converts melodic model from numerical to hexcode string form
char* headstr(short *a, short len) {
	static char str[5];
	short length = 0;
	// FIRST INT CASES
	// first int is greater than zero
	if (a[0] > 0) {
		str[length] = 'U';
		str[length + 1] = a[0] + '0';
		length += 2;
	// first int is less than zero
	} else if (a[0] < 0) {
		str[length] = 'D';
		str[length + 1] = abs(a[0]) + '0';
		length += 2;
	// first int is zero
	} else {
		str[length] = 'S';
		++length;
	}
	// SECOND INT CASES
	// second int is greater than zero
	if (a[1] > 0) {
		str[length] = 'U';
		str[length + 1] = a[1] + '0';
		length += 2;
	// second int is less than zero
	} else if (a[1] < 0) {
		str[length] = 'D';
		str[length + 1] = abs(a[1]) + '0';
		length += 2;
	// second int is zero
	} else {
		str[length] = 'S';
		++length;
	}
	str[length] = '\0';

	return str;
}