/* src/linker.ld */
ENTRY(_start)

SECTIONS
{
    . = 0x10000;

    .text : {
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
    }

    /* Убираем ненужные секции */
    /DISCARD/ : {
        *(.eh_frame)
        *(.eh_frame.*)
        *(.comment)
        *(.note.*)
        *(.reloc)
        *(.ctors)
        *(.dtors)
    }
}