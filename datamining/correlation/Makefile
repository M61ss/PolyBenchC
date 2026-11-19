CC 			:= gcc
FILENAME 	?= correlation
OUTPUT 		?= $(FILENAME).txt
INCLUDES 	:= ../../utilities
SRC 		:= $(INCLUDES)/polybench.c $(FILENAME).c 
CFLAGS    	:= -O3
LDLIBS    	:= -lm
EXT_CFLAGS 	?=

all:
	$(CC) $(CFLAGS) $(EXT_CFLAGS) -I $(INCLUDES) $(SRC) -o $(FILENAME).o $(LDLIBS)

clean:
	rm -f $(FILENAME).o $(OUTPUT)

run:
	./$(FILENAME).o

profile:
	perf stat -r 10 ./$(FILENAME).o