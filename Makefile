# the compiler: gcc for C program, define as g++ for C++
CC = clang++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -std=c++17 -stdlib=libc++ -g -Wall

# the build target executable:

EXE = 6502
TEST_EXE = test$(EXE)
BUILD_DIR = ./build
BIN_DIR = ./bin
OUT_DIRS = ${BUILD_DIR} ${BIN_DIR}

# define the C source files
SRCS = cpu.cpp mem.cpp

TESTS = test/AllTests.h \
	test/cpu/reset.h
# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.cpp=.o)

all: clean dirs main test

dirs: ${OUT_DIRS}

${OUT_DIRS}:
	mkdir -p ${OUT_DIRS}

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

main: main.cpp $(OBJS)
	$(CC) $(CFLAGS) -o ./bin/$(EXE) main.cpp  $(OBJS)
AllTests.txt:
	find ./test -name "*.h" -type f 2>/dev/null 1>./AllTests.txt

unit-tests.cpp: AllTests.txt
	lib/cxxtest-4.4/bin/cxxtestgen --error-printer -o ./unit-tests.cpp --headers=./AllTests.txt

build-test: unit-tests.cpp $(OBJS)
	$(CC) $(CFLAGS) -I./lib/cxxtest-4.4 -I. -o $(BIN_DIR)/$(TEST_EXE) unit-tests.cpp  $(OBJS)

test: build-test
	$(BIN_DIR)/$(TEST_EXE)

clean:
	rm -f unit-tests.cpp AllTests.txt
	rm -rf ./$(BIN_DIR)/* ./$(BUILD_DIR)/*

.PHONY: main
