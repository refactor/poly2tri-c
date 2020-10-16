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

%_gtest.run: test/%_gtest.c ${TEST_HEADERS} ${DATA_HEADERS} ${OBJS}
	$(CC) ${CFLAGS} ${HEADER_PATH} -g -o $@ $^ ${LDFLAGS}

src/polygon_triangulate.o: src/polygon_triangulate.c ${HEADERS}
	${CC} ${CFLAGS} ${HEADER_PATH} -g -o $@ -c $<

clean:
	-@rm -f test/*.o src/*.o *.run a.out
