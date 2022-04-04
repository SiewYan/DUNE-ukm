COMPILER=$(shell root-config --cxx)
FLAGS=$(shell root-config --cflags --libs) -g -O3 -Wall -Wextra -Wpedantic -O0
BASE_DIR=${PWD}
SRC_DIR = $(BASE_DIR)/src/
PLUGINS_DIR = $(BASE_DIR)/plugins/
INCLUDES = -I $(SRC_DIR) -I $(PLUGINS_DIR)

.PHONY = all clean makefilelist

all: postproc

postproc: src/postproc.cxx
		@[ -d $(BASE_DIR)/bin ] || mkdir -p $(BASE_DIR)/bin
		$(COMPILER) $(FLAGS) $(INCLUDES) $< -o bin/$@
makefilelist:
		cd data; python mkfilelist.py

clean:
		rm -rf core src/*.o bin/ jobs/ data/*.txt
