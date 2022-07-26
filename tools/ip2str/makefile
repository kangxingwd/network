TARGET = ip2str
TARGET_DIR = ./
SRCS    := $(wildcard *.c)
INCLUDE = ./
CC		= gcc
CXX     = g++
LDFLAGS	+= 
CFLAGS = -g $(INCLUDE:%=-I%)
TARGET_LIB ?=	$(TARGET_DIR)/bin
TARGET_INCLUDE ?=	$(TARGET_DIR)/include/base

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(TARGET)

install:
	mkdir -p $(TARGET_LIB)
	cp -rf $(TARGET) $(TARGET_LIB)/$(TARGET)

