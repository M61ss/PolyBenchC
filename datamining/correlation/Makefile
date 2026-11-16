all:
	gcc -I ../../utilities/ ../../utilities/polybench.c ./correlation.c -lm -o correlation.o

clean:
	rm *.o

run:
	./correlation.o

profile:
	perf stat ./correlation.o