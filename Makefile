CC=gcc
TARGET=build/sex
SRC=sex.c

sex:
	@mkdir -p $(dir $(TARGET))
	$(CC) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)/*.o

.PHONY: clean sex

