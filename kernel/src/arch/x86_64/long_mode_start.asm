
bits 64

section .text
global long_mode_start
extern kmain

long_mode_start:
	; Fill data segment registers with 0's to discard the offsets from the previous GDT
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call kmain
	hlt
