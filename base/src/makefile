TARGET = libbase.so
TARGET_DIR = /usr/local/
SRCS    := $(wildcard *.c)
INCLUDE = ./ ../include/
CC		= gcc
CXX     = g++
LDFLAGS	+= 
CFLAGS = -g $(INCLUDE:%=-I%) -fPIC -shared
TARGET_LIB ?=	$(TARGET_DIR)/lib/
TARGET_INCLUDE ?=	$(TARGET_DIR)/include/base
HEAD = ../include/*

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(TARGET)

install:
	cp -rf $(TARGET) $(TARGET_LIB)/$(TARGET)
	mkdir -p $(TARGET_INCLUDE)
	cp -rf $(HEAD) $(TARGET_INCLUDE)/
