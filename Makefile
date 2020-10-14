#CFLAGS += -Wall -Werror -Wextra -pedantic
LDFLAGS += -lm

CC = gcc

HEADER_PATH = -I${PWD}/include -I/usr/include

LINKED_OBJECTS = src/polygon_triangulate.o

test: gtest_one.run
	./gtest_one.run

%.run: test/%.c ${LINKED_OBJECTS}
	$(CC) ${HEADER_PATH} ${CFLAGS} -g -o $@ $^ ${LDFLAGS}

src/polygon_triangulate.o: src/polygon_triangulate.c include/polygon_triangulate.h
	${CC} ${HEADER_PATH} ${CFLAGS} -g -o $@ -c $<

clean:
	-@rm -f test/*.o src/*.o *.run a.out
