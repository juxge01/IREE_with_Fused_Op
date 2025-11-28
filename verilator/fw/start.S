    .section .init
    .globl _start
_start:
    /* 스택을 RAM 상단으로 */
    la   sp, __stack_top

    /* .bss 를 0으로 클리어 */
    la   t0, __bss_start
    la   t1, __bss_end
1:
    beq  t0, t1, 2f
    sw   zero, 0(t0)
    addi t0, t0, 4
    j    1b
2:
    /* main() 진입 */
    jal  ra, main

3:  j    3b                 /* 리턴 안 함: 무한루프 */

    .size _start, . - _start