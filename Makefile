OBJ = a_string.o
HEADERS = a_common.h a_string.h a_vector.h 

build: $(HEADERS) $(OBJ)
	ld -r $(OBJ) -o asv.o

demo:
	$(CC) $(CFLAGS) -o demo demo.c

clean:
	rm -rf $(OBJ) asv.* demo demo*

.PHONY: clean
