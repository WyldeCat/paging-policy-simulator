SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET := $(OBJ_DIR)/pps.so

TEST_DIR := tests
TEST_BIN_DIR := tests/bin
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp, $(TEST_BIN_DIR)/%, $(TEST_SRCS))

.PHONY: all clean

all: $(TARGET) $(TEST_BINS)

$(TEST_BIN_DIR):
	mkdir -p $@

$(TEST_BIN_DIR)/%: $(TEST_DIR)/%.cpp | $(TEST_BIN_DIR)
	g++ -g -std=c++14 $< -o $@ -lpthread

$(OBJ_DIR):
	mkdir -p $@

$(TARGET): $(OBJS)
	g++ -shared -Wl,--hash-style=sysv $(PIN_ROOT)/intel64/runtime/pincrt/crtbeginS.o -Wl,-Bsymbolic -Wl,--version-script=$(PIN_ROOT)/source/include/pin/pintool.ver -fabi-version=2 -o $(TARGET) $(OBJS) -L$(PIN_ROOT)/intel64/runtime/pincrt -L$(PIN_ROOT)/intel64/lib -L$(PIN_ROOT)/intel64/lib-ext -L$(PIN_ROOT)/extras/xed-intel64/lib -lpin -lxed $(PIN_ROOT)/intel64/runtime/pincrt/crtendS.o -lpin3dwarf  -ldl-dynamic -nostdlib -lstlport-dynamic -lm-dynamic -lc-dynamic -lunwind-dynamic

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	g++ -Wall -Werror -Wno-unknown-pragmas -D__PIN__=1 -DPIN_CRT=1 -fno-stack-protector -fno-exceptions -funwind-tables -fasynchronous-unwind-tables -fno-rtti -DTARGET_IA32E -DHOST_IA32E -fPIC -DTARGET_LINUX -fabi-version=2 -faligned-new  -I$(PIN_ROOT)/source/include/pin -I$(PIN_ROOT)/source/include/pin/gen -isystem $(PIN_ROOT)/extras/stlport/include -isystem $(PIN_ROOT)/extras/libstdc++/include -isystem $(PIN_ROOT)/extras/crt/include -isystem $(PIN_ROOT)/extras/crt/include/arch-x86_64 -isystem $(PIN_ROOT)/extras/crt/include/kernel/uapi -isystem $(PIN_ROOT)/extras/crt/include/kernel/uapi/asm-x86 -I$(PIN_ROOT)/extras/components/include -I$(PIN_ROOT)/extras/xed-intel64/include/xed -I$(PIN_ROOT)/source/tools/Utils -I$(PIN_ROOT)/source/tools/InstLib -O3 -fomit-frame-pointer -fno-strict-aliasing   -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) | true
	rm -rf $(TEST_BIN_DIR) | true


