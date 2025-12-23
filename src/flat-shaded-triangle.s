; -----------------------------------------------------------------------------
; Subroutine to draw a flat-shaded triangle
; Args:
; $a0 = IO_BASE_ADDR (IO ports at 0x1F90****)
; $s0 = Color (for example: 0xBBGGRR)
; $s1 = x1
; $s2 = y1
; $s3 = x2
; $s4 = y2
; $s5 = x4
; $s6 = y3
; -----------------------------------------------------------------------------
DrawFlatTriangle:
    ori  $t1, $s0, 0x2000  ; Command + Color
    sw   $t1, GP0($a0)

    sll  $s2, $s2, 16      ; Vertex 1
    andi $s1, $s1, 0xFFFF
    or   $t1, $s1, $s2
    sw   $t1, GP0($a0)

    sll  $s4, $s4, 16      ; Vertex 2
    andi $s3, $s3, 0xFFFF
    or   $t1, $s3, $s4
    sw   $t1, GP0($a0)

    sll  $s6, $s6, 16      ; Vertex 3
    andi $s5, $s5, 0xFFFF
    or   $t1, $s5, $s6
    sw   $t1, GP0($a0)

    jr, $ra
    nop

lui $a0, IO_BASE_ADDR
li $s0, 0xFFFF00
li $s1, 200
li $s2, 40
li $s3, 288
li $s4, 56
li $s5, 224
li $s6, 200
jal DrawFlatTriangle
nop