all: allocate

allocate: allocate.c function.o
	gcc -o allocate allocate.c function.o -lm

function.o: function.c
	gcc -c function.c

clean: 
	rm -f *.o allocate