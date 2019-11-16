CC=gcc

bsold: sold.c test.c 
	gcc sold.c test.c sold.h -Wall -Wextra -pedantic -ansi -lm -O2
