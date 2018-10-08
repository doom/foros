include mk/common.mk

ARCH		?=	x86_64

KERNEL		:=	build/foros-$(ARCH).bin
ISO		:=	build/foros-$(ARCH).iso

LINKER_SCRIPT	:=	kernel/src/arch/$(ARCH)/linker.ld
GRUB_CFG	:=	kernel/src/arch/$(ARCH)/grub.cfg

ASM_SRC		=	$(wildcard kernel/src/arch/$(ARCH)/*.asm)
ASM_OBJ		=	$(patsubst kernel/src/arch/$(ARCH)/%.asm, build/arch/$(ARCH)/%.o, $(ASM_SRC))

OBJ		=	$(ASM_OBJ)

all:			$(KERNEL)

ifeq ($(P),debug)
run:			$(ISO)
			@echo "Started in debug mode"
			@echo "GDB can attach through port 1234"
			@qemu-system-$(ARCH) -cdrom $(ISO) -s -S
else
run:			$(ISO)
			@qemu-system-$(ARCH) -cdrom $(ISO) -s
endif

iso:			$(ISO)

$(ISO):			$(KERNEL) $(GRUB_CFG)
			@mkdir -p build/iso/boot/grub
			@cp $(KERNEL) build/iso/boot/kernel.bin
			@cp $(GRUB_CFG) build/iso/boot/grub/grub.cfg
			@sed -i "/[ ]*multiboot/ s/$$/ $(BOOT_ARGS)/" build/iso/boot/grub/grub.cfg
			@grub-mkrescue -o $(ISO) build/iso 2>/dev/null
			@$(RM) -r build/iso

$(KERNEL):		$(OBJ) $(LINKER_SCRIPT)
			@$(ECHO) $(TITLECOLOR)[LD] $(EXECCOLOR)$(KERNEL)$(NORMAL)
			@ld -n -T $(LINKER_SCRIPT) -o $(KERNEL) $(OBJ)

build/arch/$(ARCH)/%.o:	kernel/src/arch/$(ARCH)/%.asm
			@mkdir -p $(shell dirname $@)
			@$(ECHO) $(TITLECOLOR)[AS] $(SRCSCOLOR)$<$(NORMAL)
			@nasm -felf64 $< -o $@

clean:
			@$(ECHO) $(TITLECOLOR)[RM] $(EXECCOLOR)build$(NORMAL)
			@$(RM) -r build $(OBJ)

re:			clean all

.PHONY:			all clean run iso re
