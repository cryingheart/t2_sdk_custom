MEMORY
{
	ram  (rw) : ORIGIN = 0x00040c00, LENGTH = 0x3f300
}

ENTRY(main)

SECTIONS
{
    .text : {
	    *(.text*)
            *(.rodata*)
            . = ALIGN(4); /* for sake of .data section */
    } >ram

    .data : {
            *(.data)
            *(.stack)
            . = ALIGN(4);
    } > ram

    .bss : {
	    *(.bss)
	    . = ALIGN(4);
    } > ram
}
