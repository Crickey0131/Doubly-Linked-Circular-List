exp: warmup1.o my402list.o
	gcc -o warmup1 -g warmup1.o my402list.o

exp.o: warmup1.c my402list.h my402transac.h
	gcc -g -c -Wall warmup1.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o warmup1
