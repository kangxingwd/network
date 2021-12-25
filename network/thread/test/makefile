TARGET = test_thread
SRCS    := $(wildcard *.c)
INCLUDE = ./
CC=gcc

$(TARGET): $(SRCS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(TARGET)
