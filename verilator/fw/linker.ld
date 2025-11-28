ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x30000000, LENGTH = 0x01000000 /* 16 MiB */
}

__stack_top = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .text : {
    *(.init)
    *(.text .text.*)
    *(.rodata .rodata.*)
  } > RAM

  .data : {
    __data_start = .;
    *(.data .data.*)
    __data_end = .;
  } > RAM

  .bss (NOLOAD) : {
    __bss_start = .;
    *(.bss .bss.*)
    *(COMMON)
    __bss_end = .;
  } > RAM

  /DISCARD/ : { *(.comment*) *(.eh_frame*) *(.riscv.attributes*) }
}