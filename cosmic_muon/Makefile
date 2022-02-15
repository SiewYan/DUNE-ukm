
COMPILER=$(shell root-config --cxx)
FLAGS=$(shell root-config --cflags --libs)

ntuple: analysis.cxx
		$(COMPILER) -g -O3 -Wall -Wextra -Wpedantic -o analysis analysis.cxx $(FLAGS)

clean:
		rm -rf core analysis *.o
