OBJ = a_string.o
HEADERS = a_common.h a_string.h a_vector.h 

build: $(HEADERS) $(OBJ)
	ld -r $(OBJ) -o asv.o

demos: build
	$(CC) $(CFLAGS) -fsanitize=address -o a_string_demo a_string_demo.c a_string.o
	$(CC) $(CFLAGS) -fsanitize=address -o a_vector_demo a_vector_demo.c

clean:
	rm -rf $(OBJ) asv.* demo demo*

.PHONY: clean
