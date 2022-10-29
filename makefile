# Default compiler is gcc
CC = gcc
# Compiler flags
CFLAGS = -g -Wall

default: attempt

# Link to create executable "attempt"
attempt: attempt_1.o char_io.o history.o
	$(CC) $(CFLAGS) -o attempt attempt_1.o char_io.o history.o

# Compile and assemble to create object file "attempt_1.o"
attempt_1.o: attempt_1.c
	$(CC) $(CFLAGS) -c attempt_1.c 

char_io.o: char_io.c
	$(CC) $(CFLAGS) -c char_io.c 

history.o: history.c 
	$(CC) $(CFLAGS) -c history.c 

# Cleaning procedure, type "make clean"
clean:
	$(RM) count *.o *~