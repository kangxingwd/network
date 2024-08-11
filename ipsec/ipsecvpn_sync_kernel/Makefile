export CC = ${CROSS_COMPILE}gcc
export CXX = $(CROSS_COMPILE)g++

INCLUDE=./ ./include/libnl
SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)
LIBDIR = /lib64/
CFLAGS   = -g $(INCLUDE:%=-I%)
LDFLAGS  = -Wl,--no-as-needed -L./lib -lpthread -lnl-3 -lnl-xfrm-3 -Wl,-rpath=$(LIBDIR)
TARGET_DIR_BIN ?= ./
TARGETDIR = $(TARGET_DIR_BIN)
TARGET	= $(TARGETDIR)/ipsecvpn_sync_kernel

.PHONY: clean

all:$(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(TARGETDIR)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

# gcc -g -I./ -I./include/libnl -c -o log.o log.c 
# gcc -g -I./ -I./include/libnl -c -o main.o main.c
# gcc -g -I./ -I./include/libnl log.o main.o -L./lib -lpthread -lnl-3 -lnl-xfrm-3 -Wl,-rpath=/lib64/ -o .//ipsecvpn_sync_kernel
# gcc -g -I./ -I./include/libnl log.o main.o -o .//ipsecvpn_sync_kernel -Wl,-rpath=/lib64/  -L./lib -lpthread -lnl-3 -lnl-xfrm-3 
# gcc -g -I./ -I./include/libnl log.o main.o -o .//ipsecvpn_sync_kernel -Wl,-rpath=/lib64/  -L./lib -lpthread -lnl-3 -lnl-xfrm-3  


