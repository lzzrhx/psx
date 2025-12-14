.psx
.create "factorial.bin", 0x80010000
.org 0x80010000
Main:
    li $a0, 6
    jal Factorial
    nop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to compute the factorial of a number
;; Argument: num ($a0)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Factorial:
    li $t1, 1
    li $t3, 1
    li $t4, 1
WhileOuter:
    bgt $t1, $a0, WhileOuterEnd
    nop
    li $t2, 0
    li $t4, 0
WhileInner:
    bge $t2, $t1, WhileInnerEnd
    nop
    add $t4, $t4, $t3
    addi $t2, $t2, 1
    j WhileInner
    nop
WhileInnerEnd:
    move $t3, $t4
    addi $t1, $t1, 1
    j WhileOuter
    nop
WhileOuterEnd:
    move $v0, $t4
    jr $ra

.close