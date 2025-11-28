    .section .init
    .globl _start
_start:
    la   sp, __stack_top

    // .bss clear
    la   t0, __bss_start
    la   t1, __bss_end
1:  beq  t0, t1, 2f
    sw   zero, 0(t0)
    addi t0, t0, 4
    j    1b
2:
    jal  ra, main
3:  j    3b
