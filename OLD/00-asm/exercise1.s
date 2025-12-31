.psx
.create "exercise1.bin", 0x80010000
.org 0x80010000
Main:
    li $t0, 0x0001
    li $t1, 0x0100
    li $t2, 0x0011
End:
.close