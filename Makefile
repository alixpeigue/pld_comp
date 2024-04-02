# config.mk contains the paths to antlr4 etc.
# Each student should have a config.mk corresponding to her system.
# Examples are  ubuntu.mk, DI.mk, fedora.mk
# Then config.mk should be in the .gitignore of your project
include config.mk

GRAMMAR = grammar/ifcc.g4

CC=g++
CCFLAGS=-g -c -std=c++17 -I$(ANTLRINC) -Igenerated -Isrc -Wno-attributes # -Wno-defaulted-function-deleted -Wno-unknown-warning-option
LDFLAGS=-g

SRC-DIR = src
OUT-DIR = build

.PHONY: check gui clean doc

default: all
all: ifcc

##########################################
# link together all pieces of our compiler 
ANTLR_OBJECTS=$(OUT-DIR)/ifccBaseVisitor.o \
	$(OUT-DIR)/ifccLexer.o \
	$(OUT-DIR)/ifccVisitor.o \
	$(OUT-DIR)/ifccParser.o \

SRCS = $(wildcard $(SRC-DIR)/*.cpp)
OBJECTS = $(SRCS:$(SRC-DIR)/%.cpp=$(OUT-DIR)/%.o)

ifcc: $(OBJECTS) $(ANTLR_OBJECTS)
	@mkdir -p build
	$(CC) $(LDFLAGS) $(OBJECTS) $(ANTLR_OBJECTS) $(ANTLRLIB) -o ifcc

##########################################
# compile our hand-writen C++ code: main(), CodeGenVisitor, etc.
$(OUT-DIR)/%.o: src/%.cpp generated/ifccParser.cpp
	@mkdir -p $(OUT-DIR)
	$(CC) $(CCFLAGS) -MMD -o $@ $< 

##########################################
# compile all the antlr-generated C++
 $(OUT-DIR)/%.o: generated/%.cpp $(GRAMMAR)
	@mkdir -p $(OUT-DIR)
	$(CC) $(CCFLAGS) -MMD -o $@ $< 

# automagic dependency management: `gcc -MMD` generates all the .d files for us
-include $(OUT-DIR)/*.d
$(OUT-DIR)/%.d:

##########################################
# generate the C++ implementation of our Lexer/Parser/Visitor
generated/ifccLexer.cpp: generated/ifccParser.cpp
generated/ifccVisitor.cpp: generated/ifccParser.cpp
generated/ifccBaseVisitor.cpp: generated/ifccParser.cpp
generated/ifccParser.cpp: $(GRAMMAR)
	@mkdir -p generated
	$(ANTLR) -visitor -no-listener -Dlanguage=Cpp -o generated $(GRAMMAR) -Xexact-output-dir

# prevent automatic cleanup of "intermediate" files like ifccLexer.cpp etc
.PRECIOUS: generated/ifcc%.cpp   

##########################################
# view the parse tree in a graphical window

# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= ../tests/testfiles/1_return42.c

gui:
	@mkdir -p generated build
	$(ANTLR) -Dlanguage=Java -o generated -Xexact-output-dir $(GRAMMAR)
	javac -cp $(ANTLRJAR) -d build generated/*.java
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# delete all machine-generated files
clean:
	rm -rf $(OUT-DIR) generated ifcc-test-output
	rm -f ifcc

##########################################
# Performs all tests
check : check-x86 check-rv64
check-x86: all
	python3 tests/ifcc-test.py --ifcc-wrapper tests/ifcc-wrapper-x86.sh --gcc-wrapper tests/gcc-wrapper-x86.sh --as-wrapper tests/as-wrapper-x86.sh --exec-wrapper tests/exec-wrapper-x86.sh tests/testfiles
check-rv64: all
	python3 tests/ifcc-test.py --ifcc-wrapper tests/ifcc-wrapper-rv64.sh --gcc-wrapper tests/gcc-wrapper-rv64.sh --as-wrapper tests/as-wrapper-rv64.sh --exec-wrapper tests/exec-wrapper-rv64.sh tests/testfiles

#########################################
# generate doxygen documentation
doc:
	doxygen doxygen-config
