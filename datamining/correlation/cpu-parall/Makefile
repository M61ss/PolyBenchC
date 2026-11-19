CC 			:= gcc
FILENAME 	?= correlation
GNDTRUTH  	?= ../$(FILENAME).txt
OUTPUT 		?= $(FILENAME).txt
INCLUDES 	:= ../../../utilities
SRC 		:= $(INCLUDES)/polybench.c $(FILENAME).c 
CFLAGS    	:= -O3 -fopenmp
LDLIBS    	:= -lm
EXT_CFLAGS 	?=

all:
	$(CC) $(CFLAGS) $(EXT_CFLAGS) -I $(INCLUDES) $(SRC) -o $(FILENAME).o $(LDLIBS)

clean:
	rm -f $(FILENAME).o $(OUTPUT)

run:
	./$(FILENAME).o

check:
	diff --normal $(GNDTRUTH) $(OUTPUT)

profile:
	perf stat -r 10 ./$(FILENAME).o