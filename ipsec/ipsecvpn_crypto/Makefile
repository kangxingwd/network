export CC = ${CROSS_COMPILE}gcc
export CXX = $(CROSS_COMPILE)g++

INCLUDE=./ ./gmssl_3.1.1_dep/include/
SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)
LIBDIR = /lib64/
CFLAGS   = -g $(INCLUDE:%=-I%)
LDFLAGS  = -Wl,--no-as-needed -L./gmssl_dep/lib -lpthread -lgmssl -Wl,-rpath=$(LIBDIR)
TARGET_DIR_BIN ?= ./
TARGETDIR = $(TARGET_DIR_BIN)
TARGET	= $(TARGETDIR)/ipsecvpn_crypto

.PHONY: clean

all:$(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(TARGETDIR)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

