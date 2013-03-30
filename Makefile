CC=gcc
CFLAGS=-I.
DEPS = lcpskip.h
OBJ = test.o lcpskip.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)