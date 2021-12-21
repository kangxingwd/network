# Func: Build libDebugLog.so
# Auth: zhangcan
# Date: 2017/12/7

CC      := g++ --std=c++11
TARGET  := libDebugLog-2.0.so
SRCS    := $(wildcard *.cpp)
BIN_DIR	:= ../../assemble/SetupJumpFwDom/lib/

$(TARGET): $(SRCS)
	$(CC) -shared -fPIC -o $@ $^ $(FLAGES)

.PHONY: clean install
clean:
	@rm -rf $(TARGET)
install:
	@cp -f $(TARGET) $(BIN_DIR)