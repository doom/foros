ENTRY(start)

SECTIONS {
    . = 1M; /* set the address of the first section at 1M, conventional address to load a kernel */

    .boot :
    {
        /* ensure that the multiboot header is at the beginning */
        KEEP(*(.multiboot_header))
    }

    .text :
    {
        *(.text)
    }
}
