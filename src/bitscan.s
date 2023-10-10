.syntax unified
.arch armv7-a

.section .text
.global _BitScanReverse

_BitScanReverse:
    mov     r2, #31             @ Initialize the loop counter to 31
    mov     r3, r0              @ Copy the input value to r3
    mov     r1, #0              @ Initialize the result (bit position) to 0

loop:
    tst     r3, r3, lsr #1      @ Test the least significant bit
    bne     found               @ If it's set, exit the loop

    sub     r2, r2, #1          @ Decrement the loop counter
    lsr     r3, r3, #1          @ Shift the input value right by 1
    bpl     loop                @ Repeat the loop if r3 is still positive

not_found:
    mov     r0, #-1             @ Set r0 to -1 if no bit is set
    bx      lr

found:
    mov     r0, r1              @ Move the result (bit position) to r0
    bx      lr 
