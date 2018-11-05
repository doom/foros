include mk/common.mk

USE_SSE			?=	no
USE_BUILTIN_INTERRUPT	?=	yes

ARCH			?=	x86_64

KERNEL			:=	build/foros-$(ARCH).bin
ISO			:=	build/foros-$(ARCH).iso

LINKER_SCRIPT		:=	kernel/src/arch/$(ARCH)/linker.ld
GRUB_CFG		:=	kernel/src/arch/$(ARCH)/grub.cfg

ASM_SRC			=	$(wildcard kernel/src/arch/$(ARCH)/*.asm)
ASM_OBJ			=	$(patsubst kernel/src/arch/$(ARCH)/%.asm, build/src/arch/$(ARCH)/%.o, $(ASM_SRC))

CXX_SRC			:=	$(wildcard kernel/src/*.cpp) \
				$(wildcard kernel/src/*/*.cpp) \
				$(wildcard kernel/tests/*.cpp)
CXX_OBJ			:=	$(patsubst kernel/%.cpp, build/%.o, $(CXX_SRC))
CXX_DEP			:=	$(CXX_OBJ:.o=.d)

LIBCPATH		:=	lib/for_libcxx/lib/for_libc
LIBCXXPATH		:=	lib/for_libcxx/

CPPFLAGS		=	-isystem $(LIBCPATH)/include -isystem $(LIBCPATH)/include/$(ARCH) -isystem $(LIBCXXPATH)/include
CPPFLAGS		+=	-Ilib/strong_type/include -Ikernel/include -I$(LIBCPATH)/lib/ut/include

CXXFLAGS		=	-Wall -Wextra -nostdinc -nostdlib -fno-builtin -O3 -std=c++17
CXXFLAGS		+=	-fno-rtti -fno-exceptions \
				-fno-threadsafe-statics \
				-mno-ssse3 \
				-mno-sse4 \
				-mno-sse4.1 \
				-mno-sse4.2 \
				-mno-red-zone \
				-fno-stack-protector

ifneq ($(USE_SSE),yes)
CXXFLAGS		+=	-mno-sse \
				-mno-sse2 \
				-mno-sse3
endif

ifeq ($(USE_BUILTIN_INTERRUPT),yes)
CXXFLAGS		+=	-mgeneral-regs-only
CPPFLAGS		+=	-DFOROS_USE_BUILTIN_INTERRUPT
endif

OBJ			=	$(ASM_OBJ) $(CXX_OBJ)

all:				$(KERNEL)

ifeq ($(P),debug)
CXXFLAGS		+=	-g3

run:				$(ISO)
				@echo "Started in debug mode"
				@echo "GDB can attach through port 1234"
				@qemu-system-$(ARCH) -cdrom $(ISO) -s -S
else
run:				$(ISO)
				@qemu-system-$(ARCH) -cdrom $(ISO) -s
endif

iso:				$(ISO)

$(ISO):				$(KERNEL) $(GRUB_CFG)
				@mkdir -p build/iso/boot/grub
				@cp $(KERNEL) build/iso/boot/kernel.bin
				@cp $(GRUB_CFG) build/iso/boot/grub/grub.cfg
				@sed -i "/[ ]*multiboot/ s/$$/ $(BOOT_ARGS)/" build/iso/boot/grub/grub.cfg
				@grub-mkrescue -o $(ISO) build/iso 2>/dev/null
				@$(RM) -r build/iso

for_libcxx:
				$(MAKE) -C lib/for_libcxx


$(KERNEL):			for_libcxx $(OBJ) $(LINKER_SCRIPT)
				@$(ECHO) $(TITLECOLOR)[LD] $(EXECCOLOR)$(KERNEL)$(NORMAL)
				@ld -n -T $(LINKER_SCRIPT) -o $(KERNEL) $(OBJ) -Llib/for_libcxx/lib/for_libc -lfor_libc-static -Llib/for_libcxx -lfor_libcxx-static

build/src/arch/$(ARCH)/%.o:	kernel/src/arch/$(ARCH)/%.asm
				@mkdir -p $(shell dirname $@)
				@$(ECHO) $(TITLECOLOR)[AS] $(SRCSCOLOR)$<$(NORMAL)
				@nasm -felf64 $< -o $@

-include $(CXX_DEP)

build/%.o:			kernel/%.cpp
				@mkdir -p $(shell dirname $@)
				@$(ECHO) $(TITLECOLOR)[CC] $(SRCSCOLOR)$<$(NORMAL)
				@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -MMD -MP

clean:
				$(MAKE) -C lib/for_libcxx clean
				@$(ECHO) $(TITLECOLOR)[RM] $(EXECCOLOR)build$(NORMAL)
				@$(RM) -r build $(OBJ)

re:				clean all

.PHONY:				all clean run iso re
