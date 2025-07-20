OBJ = a_string.o a_vector.o a_slice.o
HEADERS = a_common.h a_string.h a_vector.h a_slice.h

build: $(HEADERS) $(OBJ)
	ld -r $(OBJ) -o asv.o

demo:
	$(CC) $(CFLAGS) -o demo demo.c

clean:
	rm -rf $(OBJ) asv.* demo demo*

.PHONY: clean
