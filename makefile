CC=g++
CFLAGS=-static-libgcc -static-libstdc++ -Wall

LIBS=-lpulse -lpulse-simple

pulse-test: pulse-test.o
	$(CC) $(CFLAGS) -o "pulse-test" pulse-test.o $(LIBS)
	
clean:
	rm -f *.o

