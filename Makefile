# -*- MakeFile -*-

2x2: 2x2.o diatonic.o
	cc 2x2.o diatonic.o -o 2x2 -l m

2x2.o: 2x2.c diatonic.h
	cc -c 2x2.c

diatonic.o: diatonic.c
	cc -c diatonic.c

clean:
	rm *.o
