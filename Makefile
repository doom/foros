include mk/common.mk

ARCH			?=	x86_64

KERNEL			:=	build/foros-$(ARCH).bin
ISO			:=	build/foros-$(ARCH).iso

LINKER_SCRIPT		:=	kernel/src/arch/$(ARCH)/linker.ld
GRUB_CFG		:=	kernel/src/arch/$(ARCH)/grub.cfg

ASM_SRC			=	$(wildcard kernel/src/arch/$(ARCH)/*.asm)
ASM_OBJ			=	$(patsubst kernel/src/arch/$(ARCH)/%.asm, build/src/arch/$(ARCH)/%.o, $(ASM_SRC))

CXX_SRC			:=	$(wildcard kernel/src/*.cpp) \
				$(wildcard kernel/src/*/*.cpp)
CXX_OBJ			:=	$(patsubst kernel/src/%.cpp, build/src/%.o, $(CXX_SRC))
CXX_DEP			:=	$(CXX_OBJ:.o=.d)

CPPFLAGS		=	-Ikernel/include -isystem lib/for_libcxx/include -isystem lib/for_libcxx/lib/for_libc/include -isystem lib/for_libcxx/lib/for_libc/include/$(ARCH)
CXXFLAGS		=	-Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-rtti -fno-exceptions -O3 -std=c++17

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

build/src/%.o:			kernel/src/%.cpp
				@mkdir -p $(shell dirname $@)
				@$(ECHO) $(TITLECOLOR)[CC] $(SRCSCOLOR)$<$(NORMAL)
				@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -MMD -MP

clean:
				$(MAKE) -C lib/for_libcxx clean
				@$(ECHO) $(TITLECOLOR)[RM] $(EXECCOLOR)build$(NORMAL)
				@$(RM) -r build $(OBJ)

re:				clean all

.PHONY:				all clean run iso re
