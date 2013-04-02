CC=gcc
CFLAGS=-I. -O4 -DNDEBUG
DEPS = lcpskip.h
OBJ = test.o lcpskip.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)