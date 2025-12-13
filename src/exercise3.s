.psx
.create "exercise3.bin", 0x80010000
.org 0x80010000
Main:
    li $t0, 0x001B
    li $t1, 0x0003
    li $t2, 0x0000
    j Cond
    nop
Loop:
    sub $t0, $t0, $t1
    addi $t2, $t2, 0x0001
Cond:
    bge $t0, $t1, Loop
    nop
.close