MEMORY
{
    ram  (rw)  : ORIGIN = 0x00042000, LENGTH = 0x7f300
    virt (rx)  : ORIGIN = 0x00100000, LENGTH = 0x100000
    flash (rx) : ORIGIN = 0x02000000, LENGTH = 0x100000
    hwreg (rw) : ORIGIN = 0x00fc0000, LENGTH = 0x40000
}

ENTRY(_start)

SECTIONS
{
    .text : {
        PROVIDE(__vector_table__ = .);
        KEEP(*(.vectors))
        PROVIDE(__initcall_table__ = .);
        KEEP(*(.rodata.initcall))
        KEEP(*(.text.init0))
        KEEP(*(.text.init1))
        KEEP(*(.text.init2))
        KEEP(*(.text.init3))
        KEEP(*(.text.suspend3))
        KEEP(*(.text.suspend2))
        KEEP(*(.text.suspend1))
        KEEP(*(.text.suspend0))
        KEEP(*(.text.resume0))
        KEEP(*(.text.resume1))
        KEEP(*(.text.resume2))
        KEEP(*(.text.resume3))
        KEEP(*(.text.coredump0))
        KEEP(*(.text.coredump1))
        KEEP(*(.text.coredump2))
        KEEP(*(.text.coredump3))
        PROVIDE(__initcall_end__ = .);
        __notrace_start__ = .;
        *(.text.notrace)
        __notrace_end__ = .;
        */libgcc.a:*(.text* .rodata*)
        */libc.a:*(.text* .rodata*)
        */libkernel.a:*(.text* .rodata*)
        *(.text.irq*) *(.rodata.irq*)
        *(.text.cop*) *(.rodata.cop*)
        *(.text.pr*)
        PROVIDE(__syscall_table__ = .);
        *(.syscall)
        . = ALIGN(4); /* for sake of .data section */
    } >ram

    /DISCARD/ : {
        *(.ARM.exidx)
    }

    .data : {
        PROVIDE(__data_start_rom__ = .);
        PROVIDE(__data_start__ = .);
        *(.data*)
        *(.stack*)
        . = ALIGN(4);
        PROVIDE(__data_end__ = .);
    } > ram

    .bss : {
        PROVIDE(__bss_start__ = .);
        *(.bss*)
        . = ALIGN(4);
        PROVIDE(__bss_end__ = .);
    } > ram

    .virt : {
        PROVIDE(__app_virt_start = .);
        *(.text*)
        *(.rodata*)
        PROVIDE(__app_virt_end = .);
    } >virt AT> flash

    INCLUDE hwreg.lds
}
