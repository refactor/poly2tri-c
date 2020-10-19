.PHONY: clean test

CFLAGS += -std=c11
CFLAGS += -Wall -Werror -Wextra
LDFLAGS += -lm

CC = gcc

HEADER_PATH = -I${PWD}/include -I/usr/include

SRC = $(wildcard src/*.c)
OBJS = $(SRC:%.c=%.o)
TEST_HEADERS = $(wildcard test/*_gtest.h)
DATA_HEADERS = $(wildcard test/*_data.h)
HEADERS = $(wildcard include/*.h)

#CFLAGS += -DUSING_DOUBLE

test: all_gtest.run
	./$<

%_gtest.run: test/%_gtest.c test/poly2tri_impl.c ${TEST_HEADERS} ${DATA_HEADERS} ${OBJS}
	$(CC) ${CFLAGS} ${HEADER_PATH} -g -o $@ $^ ${LDFLAGS}

clean:
	-@rm -f test/*.o src/*.o *.run a.out
