ENTRY(_start)

MEMORY {
  RAM (wx) : ORIGIN = 0x30000000, LENGTH = 0x01000000  /* 16MB */
}

SECTIONS {
  .text : {
    *(.init)
    *(.text .text.*)
    *(.rodata .rodata.*)
  } > RAM

  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  } > RAM

  .bss (NOLOAD) : {
    __bss_start = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    __bss_end = .;
  } > RAM

  . = ALIGN(8);
  __stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
