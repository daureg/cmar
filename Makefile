rm=/bin/rm -f
VALGRIND=valgrind --tool=memcheck --track-origins=yes
CC?=cc
DEFS=
INCLUDES=
LIBS=$(shell pkg-config --libs matio)
ZCOV_DIR=zcov
BIN_DIR=bin
BIN_ARGS=test.mat

DEFINES=$(INCLUDES) $(DEFS)
DEBUG=1
WARNINGS=-Wall -Wextra -pedantic
COMMON=-std=c99 $(DEFINES) $(WARNINGS) $(shell pkg-config --cflags matio)
DEBUG_FLAG=-O0 -ggdb3 -fprofile-arcs -ftest-coverage -D__DEBUG_
RELEASE_FLAGS=-march=native -mtune=native -O3
CFLAGS=$(COMMON)
ifeq ($(DEBUG), 1)
	CFLAGS+=$(DEBUG_FLAG)
else
	CFLAGS+=$(RELEASE_FLAGS)
endif

all: main

x: all
	$(BIN_DIR)/main $(BIN_ARGS)

main: main.o cmar.o
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main main.c cmar.o memsys.o $(LIBS)

memsys.o: memsys.c
	$(CC) $(CFLAGS) -c memsys.c $(LIBS)
cmar.o: cmar.c memsys.o
	$(CC) $(CFLAGS) -c cmar.c $(LIBS)

# Need check framework: http://check.sourceforge.net/
check_cmar.o: check_cmar.c
	$(CC) $(CFLAGS) -c check_cmar.c

check_cmar: check_cmar.o cmar.o
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/check_cmar check_cmar.o cmar.o memsys.o $(LIBS) $(shell pkg-config --libs check)

check: check_cmar
	$(BIN_DIR)/check_cmar

# Need zcov script: https://github.com/ddunbar/zcov
cov: check
	LANG=C $(ZCOV_DIR)/zcov-scan out.zcov .
	LANG=C $(ZCOV_DIR)/zcov-genhtml out.zcov covreport

cov-clean:
	rm -rf covreport/
	$(rm) out.zcov
	find -name "*.gc*" -delete

# Need valgrind tool: http://valgrind.org/
valgrind: check_cmar
	CK_FORK=no $(VALGRIND) $(BIN_DIR)/check_cmar

x-valgrind: all
	$(VALGRIND) $(BIN_DIR)/main $(BIN_ARGS)

clean: cov-clean
	$(rm) *.o core *~
	rm -rf $(BIN_DIR)

astyle:
	astyle -r -n "*.c" "*.h"

dist: check clean
	git archive --prefix=cmar_336978/ -o cmar_336978.tar.gz HEAD
