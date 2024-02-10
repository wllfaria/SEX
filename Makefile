# Define compiler
CC=gcc

# Define any compile-time flags
CFLAGS=-Wall -Wextra -g

# Define the target executable name
TARGET=sex

# Define source files
SRC=sex.c

# Define object files
OBJ=$(SRC:.c=.o)

# Rule for making the target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Rule for cleaning up
clean:
	rm -f $(TARGET) $(OBJ)

# Rule for compiling source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: clean

