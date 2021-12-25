TARGET = libthreadpool.so
SRCS    := $(wildcard *.c)
INCLUDE = ./ ../include/
CC		= gcc
CXX     = g++
LDFLAGS	+= -lpthread
CFLAGS = -g $(INCLUDE:%=-I%) -fPIC -shared
TARGET_LIB ?=	/usr/local/lib/
TARGET_INCLUDE ?=	/usr/local/include/
HEAD = ../include/*

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(TARGET)

install:
	cp -rf $(TARGET) $(TARGET_LIB)/$(TARGET)
	cp -rf $(HEAD) $(TARGET_INCLUDE)/
