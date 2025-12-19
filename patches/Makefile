TARGET = patches.elf

CC      ?= clang
LD      ?= ld.lld

GBI_DEFINE := -DF3DEX_GBI
CFLAGS   := -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg -mno-check-zero-division \
			-fomit-frame-pointer -ffast-math -fno-unsafe-math-optimizations -fno-builtin-memset -funsigned-char -fno-builtin-sinf -fno-builtin-cosf \
			-Wall -Wextra -Wno-incompatible-library-redeclaration -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable -Wno-missing-braces -Wno-unsupported-floating-point-opt -Wno-cast-function-type-mismatch
CPPFLAGS := -nostdinc -D_LANGUAGE_C -DMIPS -I ../lib/bk-decomp/include -I ../lib/bk-decomp/include/2.0L -I ../lib/bk-decomp/include/2.0L/PR -I../lib/rt64/include -I../lib/N64ModernRuntime/ultramodern/include $(GBI_DEFINE)
LDFLAGS  := -nostdlib -T patches.ld -T syms.ld -Map patches.map --unresolved-symbols=ignore-all --emit-relocs

C_SRCS := $(wildcard *.c)
C_OBJS := $(C_SRCS:.c=.o)
C_DEPS := $(C_SRCS:.c=.d)

$(TARGET): $(C_OBJS) patches.ld syms.ld
	$(LD) $(C_OBJS) $(LDFLAGS) -o $@

$(C_OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -MMD -MF $(@:.o=.d) -c -o $@

clean:
	rm -rf $(C_OBJS) $(TARGET) $(C_DEPS)

-include $(C_DEPS)

.PHONY: clean
