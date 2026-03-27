SECTIONS {
    . = 0x7E00;
    .text : {
        *(.text.boot)
        *(.text)
    }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
    /DISCARD/ : { *(.eh_frame) *(.comment) }
}
