section .multiboot_header

; Multiboot 2 compliant header
; See https://wiki.osdev.org/Multiboot#Header_Format
header_start:
	dd 0xE85250D6			; magic number for MultiBoot 2
	dd 0				; architecture (0 => i386)
	dd header_end - header_start	; header length

	; checksum
	dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start))

	; The header can contain a list of tags
	; Those tags can be used to provide or request information to/from the bootloader

	; Empty tag, marking the end of the tag list
	dw 0				; type
	dw 0				; flags
	dd 8				; size
header_end:
