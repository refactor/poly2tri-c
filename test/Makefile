.PHONY: clean test

TEST_HEADERS = $(wildcard *_gtest.h)
DATA_HEADERS = $(wildcard *_data.h)

CFLAGS += ${HEADER_INC} ${TEST_INC}

%_gtest.run: %_gtest.c poly2tri_impl.c ${DATA_HEADERS} 
	$(CC) ${CFLAGS} -g -o $@ $^ ${LDFLAGS}

test: all_gtest.run
	@echo "TEST_INC: ${TEST_INC}" 
	./$<

clean:
	-@rm -f *.o *.run

pnt:
	-@echo "HEADER_INC: ${HEADER_INC}"
	-@echo "DATA_HEADERS: ${DATA_HEADERS}"
	-@echo "CFLAGS: ${CFLAGS}"
	-@echo "proj-dir: $(ProjDir)"
