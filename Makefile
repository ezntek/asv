OBJ = a_string.o a_vector.o
HEADERS = a_common.h a_string.h a_vector.h

build: $(HEADERS) $(OBJ)
	ld -r $(OBJ) -o asv.o

clean:
	rm -rf $(OBJ) asv.*

.PHONY: clean
