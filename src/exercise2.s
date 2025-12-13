.psx
.create "exercise2.bin", 0x80010000
.org 0x80010000
Main:
    li $t0, 0x0001
    li $t1, 0x0000
Loop:
    add  $t1, $t0, $t1
    addi $t0, $t0, 0x0001
    ble  $t0, 0x000A, Loop
    nop
.close