CC 			:= gcc
FILENAME 	?= correlation
GNDTRUTH  	?= ../$(FILENAME).txt
OUTPUT 		?= $(FILENAME).txt
INCLUDES 	:= ../../../utilities
SRC 		:= $(INCLUDES)/polybench.c $(FILENAME).c 
CFLAGS    	:= -O3 -fopenmp -foffload=nvptx-none -fcf-protection=none -fno-stack-protector
LDLIBS    	:= -foffload-options=-lm -lm
EXT_CFLAGS 	?=

export OMP_TARGET_OFFLOAD := MANDATORY

all:
	$(CC) $(CFLAGS) $(EXT_CFLAGS) -I $(INCLUDES) $(SRC) -o $(FILENAME).exe $(LDLIBS)

clean:
	rm -f $(FILENAME).exe $(OUTPUT)

run:
	./$(FILENAME).exe

check:
	diff --normal $(GNDTRUTH) $(OUTPUT)

profile:
	perf stat -r 10 ./$(FILENAME).exe