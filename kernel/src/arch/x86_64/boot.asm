
bits 32

section .text
global start
global p4_table_addr
extern long_mode_start

start:
	mov esp, stack_top
	mov edi, ebx

	call assert_multiboot
	call assert_cpuid
	call assert_long_mode
	call assert_sse

	call setup_page_tables
	call setup_recursive_mapping
	call enable_paging
	call enable_sse

	lgdt [gdt64.pointer]

	jmp gdt64.code:long_mode_start

fatal_error:
	; https://en.wikipedia.org/wiki/VGA-compatible_text_mode
	mov DWORD [0xb8000], 0x4f524f45		; 4f: White/Red, 53: 'R', 4f, 45: 'E'
	mov DWORD [0xb8004], 0x4f3a4f52		; 4f: White/Red, 3a: ':', 4f, 53: 'E'
	mov DWORD [0xb8008], 0x4f204f20		; 4f: White/Red, 20: ' ', 4f, 20: ' '
	mov BYTE [0xb800a], al
	hlt

assert_multiboot:
	cmp eax, 0x36d76289	; Magic number dropped in EAX by multiboot2-compliant bootloaders
	jne .no_multiboot
	ret
.no_multiboot:
	mov al, '0'
	jmp fatal_error

assert_cpuid:
	; Basically attempt to flip the 21st bit. If it succeeds, cpuid is available
	; https://en.wikipedia.org/wiki/CPUID

	; Save flags to eax
	pushfd
	pop eax

	mov ecx, eax

	xor eax, 1 << 21

	push eax
	popfd

	pushfd
	pop eax

	push ecx
	popfd

	cmp eax, ecx
	je .no_cpuid
	ret
.no_cpuid:
	mov al, '1'
	jmp fatal_error

assert_long_mode:
	; Test if extended processor info is available through cpuid
	; We first ask cpuid to give us the highest value supported for an argument
	; If it is below 0x80000001, the extended info is not supported
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .no_long_mode

	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .no_long_mode
	ret

.no_long_mode:
	mov al, '2'
	jmp fatal_error

; https://en.wikipedia.org/wiki/CPUID
assert_sse:
	mov eax, 0x1		; eax=1 for processor information/features
	cpuid
	test edx, 1 << 25	; 25th bit is set if SSE is available
	jz .no_sse
	test edx, 1 << 26	; 26th bit is set if SSE2 is available
	jz .no_sse
	test ecx, 1 << 0	; 0th bit is set if SSE3 is available
	jz .no_sse
	ret

.no_sse:
	mov al, '3'
	jmp fatal_error

; https://wiki.osdev.org/SSE#Adding_support
; https://en.wikipedia.org/wiki/Control_register
enable_sse:
	mov eax, cr0
	and ax, 0xFFFB		; clear coprocessor emulation CR0.EM
	or ax, 0x2		; set coprocessor monitoring  CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, eax
	ret

; The following setup will add a P4, a P3, a P2, and 512 "huge" 2-MB pages, so 2MB * 512 -> 1GB
; It is an identity mapping, i.e. virtual addresses with be identical to real addresses
setup_page_tables:
	; Add P3 as the first entry in P4
	mov eax, p3_table
	or eax, 0b11		; Present = 1, Writable = 1 << 1
	mov [p4_table], eax

	; Add P2 as the first entry in P3
	mov eax, p2_table
	or eax, 0b11		; Present = 1, Writable = 1 << 1
	mov [p3_table], eax

	; Add 512 "huge" pages entries in P2
	mov ecx, 0
.map_2mb_page:
	mov eax, 0x200000		; Size of a page
	mul ecx				; We multiply by the number of added pages
	or eax, 0b10000011		; Present = 1, Writable = 1 << 1, Huge = 1 << 7
	mov [p2_table + ecx * 8], eax	; Add to the page

	inc ecx
	cmp ecx, 512			; We want to fill the P2 table, so loop for 512 entries
	jne .map_2mb_page
	ret

setup_recursive_mapping:
	; Load the address of the P4 table itself at the 511th entry of itself
	; This allows mapping the page tables themselves without having to identity-map
	; nor use temporary mappings
	;
	; Usually, with an address like abcde we can access memory by doing:
	; P4[a] -> P3[b] -> P2[c] -> P1[d] -> memory[e]
	; But if a = 511, we go through P4 twice and we can access data in a P1 table by doing:
	; P4[a] -> P4[b] -> P3[c] -> P2[d] -> P1[e]
	mov eax, p4_table
	or eax, 0b11			; Present = 1, Writable = 1 << 1
	mov [p4_table + 511 * 8], eax

enable_paging:
	; Load the P4 table in CR3 so the CPU can get it from there
	mov eax, p4_table
	mov cr3, eax

	; Add the Physical Address Extension to CR4
	; https://en.wikipedia.org/wiki/Physical_Address_Extension
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; Set the long mode bit in the EFER MSR (model specific register)
	; We use ecx to "point" to the EFER register
	; then rdmsr/wrmsr to read/write to it
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; Add the long mode bit to CR0
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	ret

section .rodata
gdt64:
	dq 0			; a GDT needs to start with an empty entry

	; 43 = executable, 44 = descriptor flag, 47 = "present" flag, 53 = "64-bit segment" flag
.code:	equ $ - gdt64
	dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
	; The GDT's length - 1 (i.e. the distance between here and the beginning, since we're right after the GDT)
	dw $ - gdt64 - 1
	; The GDT's address
	dq gdt64

p4_table_addr:
	dq 0xfffffffffffff000

section .bss
align 4096
p4_table:
	resb 4096
p3_table:
	resb 4096
p2_table:
	resb 4096
stack_bottom:
	resb 4096 * 4
stack_top:
