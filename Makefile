# make directives for boxstats
SCRIPTS=./scripts

all: process-cpu

process-cpu:
	gcc -Wall -std=gnu99 src/process-cpu.c -o $(SCRIPTS)/process-cpu

clean:
	rm -f $(SCRIPTS)/process-cpu
