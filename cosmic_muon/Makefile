COMPILER=$(shell root-config --cxx)
FLAGS=$(shell root-config --cflags --libs)  -g -O3 -Wall -Wextra -Wpedantic
BASE_DIR=${PWD}
SRC_DIR = $(BASE_DIR)/src/
#INCLUDES = -I $(SRC_DIR) -I $(PLUGINS_DIR)

.PHONY = all clean

all: postproc

postproc: src/postproc.cxx
		@[ -d $(BASE_DIR)/bin ] || mkdir -p $(BASE_DIR)/bin
		$(COMPILER) $(FLAGS) $< -o bin/$@

clean:
		rm -rf core src/*.o bin/
