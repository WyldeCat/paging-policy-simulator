TEST_DIR := tests
TEST_BIN_DIR := tests/bin
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp, $(TEST_BIN_DIR)/%, $(TEST_SRCS))

.PHONY: all clean

all: obj/pinatrace.so $(TEST_BINS)

$(TEST_BIN_DIR):
	mkdir -p $@

$(TEST_BIN_DIR)%: $(TEST_DIR)/%.cpp | $(TEST_BIN_DIR)
	g++ -g -std=c++14 $< -o $@ -lpthread

clean:
	rm obj/pinatrace.so | true
	rm obj/pinatrace.o | true
	rm -rf $(TEST_BIN_DIR) | true

obj/pinatrace.so: obj/pinatrace.o
	g++ -shared -Wl,--hash-style=sysv $(PIN_ROOT)/intel64/runtime/pincrt/crtbeginS.o -Wl,-Bsymbolic -Wl,--version-script=$(PIN_ROOT)/source/include/pin/pintool.ver -fabi-version=2    -o obj/pinatrace.so obj/pinatrace.o -L$(PIN_ROOT)/intel64/runtime/pincrt -L$(PIN_ROOT)/intel64/lib -L$(PIN_ROOT)/intel64/lib-ext -L$(PIN_ROOT)/extras/xed-intel64/lib -lpin -lxed $(PIN_ROOT)/intel64/runtime/pincrt/crtendS.o -lpin3dwarf  -ldl-dynamic -nostdlib -lstlport-dynamic -lm-dynamic -lc-dynamic -lunwind-dynamic

obj/pinatrace.o:
	g++ -Wall -Werror -Wno-unknown-pragmas -D__PIN__=1 -DPIN_CRT=1 -fno-stack-protector -fno-exceptions -funwind-tables -fasynchronous-unwind-tables -fno-rtti -DTARGET_IA32E -DHOST_IA32E -fPIC -DTARGET_LINUX -fabi-version=2 -faligned-new  -I$(PIN_ROOT)/source/include/pin -I$(PIN_ROOT)/source/include/pin/gen -isystem $(PIN_ROOT)/extras/stlport/include -isystem $(PIN_ROOT)/extras/libstdc++/include -isystem $(PIN_ROOT)/extras/crt/include -isystem $(PIN_ROOT)/extras/crt/include/arch-x86_64 -isystem $(PIN_ROOT)/extras/crt/include/kernel/uapi -isystem $(PIN_ROOT)/extras/crt/include/kernel/uapi/asm-x86 -I$(PIN_ROOT)/extras/components/include -I$(PIN_ROOT)/extras/xed-intel64/include/xed -I$(PIN_ROOT)/source/tools/Utils -I$(PIN_ROOT)/source/tools/InstLib -O3 -fomit-frame-pointer -fno-strict-aliasing   -c -o obj/pinatrace.o src/pinatrace.cpp

