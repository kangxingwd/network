TARGET = encrypt.so
SRCS    := $(wildcard *.c)
INCLUDE = ./
CC=gcc

$(TARGET): $(SRCS)
	$(CC) -fPIC -shared -o $@ $^

.PHONY: clean
clean:
	rm -rf $(TARGET)

install:
	cp -rf $(TARGET) /usr/local/lib/

