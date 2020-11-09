.PHONY: clean test etest

override CFLAGS = -std=c11
override CFLAGS += -Wall -Werror -Wextra
#override CFLAGS += -g

LDFLAGS += -lm

CC = gcc

ifeq (0,${MAKELEVEL})
ProjDir := $(shell pwd)
export ProjDir
endif

HEADER_INC = -I${PWD}/include
TEST_INC = -I${PWD}/test/include -I${PWD}/test/data

SRC = $(wildcard src/*.c)
OBJS = $(SRC:%.c=%.o)

## default coord_t: float
#override CFLAGS += -DUSING_DOUBLE_COORD

## default index_t: int16_t
#override CFLAGS += -DUSING_INT32_INDEX

export HEADER_INC
export TEST_INC

export CFLAGS
export LDFLAGS

test: btest etest

btest:
	cd test/jburkardt_test && $(MAKE) test
	
etest:
	cd test/earcut_test && $(MAKE) test

clean:
	-@rm -f *.o src/*.o *.run *.d a.out
	cd test/jburkardt_test && $(MAKE) clean
	cd test/earcut_test && $(MAKE) clean


pnt:
	-@echo "proj-dir: $(ProjDir)"
	cd test && $(MAKE) pnt
