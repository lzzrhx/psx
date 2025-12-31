.psx
.create "hellogpu.bin",0x80010000

; Entry Point of Code
.org 0x80010000

; -----------------------------------------------------------------------------
; IO Port
; -----------------------------------------------------------------------------
IO_BASE_ADDR equ 0x1F80         ; IO Ports Memory map base address

; -----------------------------------------------------------------------------
; GPU Registers
; -----------------------------------------------------------------------------
GP0 equ 0x1810                  ; GP0 @ $1F801810: Render data & VRAM access
GP1 equ 0x1814                  ; GP1 @ $1F801814: Display control & environment setup

Main:
    lui $a0, IO_BASE_ADDR       ; t0 = I/O port base address (mapped at 0x1F90****)

    ; -------------------------------------------------------------------------
    ; Send commands to GP1 (mapped at 0x1F801814)
    ; The GP1 is for display control and environment setuo
    ; (Command = 8-bit MSB, Parameter = 24-bit LSB)
    ; CCPPPPPP:  CC=Command  PPPPPP=Parameter
    ; -------------------------------------------------------------------------
    li $t1, 0x00000000          ; Command 00 = Reset GPU
    sw $t1, GP1($a0)            ; Writes the packet to GP1
    li $t1, 0x03000000          ; Command 03 = Display enable
    sw $t1, GP1($a0)            ; Write to GP1
    li $t1, 0x08000001          ; Command 08 = Display mode (320x240, 15-bit, NTSC)
    sw $t1, GP1($a0)            ; Write to GP1
    li $t1, 0x06C60260          ; Command 06 = Horizontal display range 0xXXXxxx (3168-608)
    sw $t1, GP1($a0)            ; Write to GP1
    li $t1, 0x07042018          ; Command 07 = Vertical display range %yyyyyyyyyyYYYYYYYYYY (264-24)
    sw $t1, GP1($a0)            ; Write to GP1


    ; -------------------------------------------------------------------------
    ; Send commands to GP0 (mapped at 0x1F801810)
    ; These GP0 commands are to setup the drawing area
    ; (Command = 8-bit MSB, Parameter = 24-bit LSB)
    ; CCPPPPPP:  CC=Command  PPPPPP=Parameter
    ; -------------------------------------------------------------------------
    li $t1, 0xE1000400          ; Command E1 = Draw mode settings
    sw $t1, GP0($a0)            ; Write to GP0
    li $t1, 0xE3000000          ; Command E3 = Drawing area top-left %YYYYYYYYYYXXXXXXXXXX (10 bits for X and 10 bits for Y)
    sw $t1, GP0($a0)            ; Write to GP0 (x=0, y=0)
    li $t1, 0xE403BD3F          ; Command E4 = Drawing area bottom-right %YYYYYYYYYYXXXXXXXXXX (10 bits for X and 10 bits for Y)
    sw $t1, GP0($a0)            ; Write to GP0 (x=319, y=239)
    li $t1, 0xE5000000          ; Command E5 = Drawing offset %YYYYYYYYYYXXXXXXXXXX (10 bits for X and 10 bits for Y)
    sw $t1, GP0($a0)            ; Write to GP0 (x=0, y=0)

    ; -------------------------------------------------------------------------
    ; Clear the screen
    ; -------------------------------------------------------------------------
    li $t1, 0x02000000          ; Command 02 = Fill rectangle in VRAM
    sw $t1, GP0($a0)            ; Write to GP0 (color=000000)
    li $t1, 0x00000000          ; Fill rectangle parameter: Top-left corner
    sw $t1, GP0($a0)            ; Write to GP0 (x=0, y=0)
    li $t1, 0x00EF013F          ; Fill rectangle parameter 3: Width & Height
    sw $t1, GP0($a0)            ; Write to GP0 (x=0, y=0)

    ; -------------------------------------------------------------------------
    ; Draw a gouraud-shaded triangle
    ; -------------------------------------------------------------------------
    li $t1, 0x300000FF          ; Command 30 = Shaded three-point polygon
    sw $t1, GP0($a0)
    li $t1, 0x001000A0          ; Vertex 1
    sw $t1, GP0($a0)
    li $t1, 0x0000FF00          ; Color2
    sw $t1, GP0($a0)
    li $t1, 0x00EF013F          ; Vertex 2
    sw $t1, GP0($a0)
    li $t1, 0x00FF0000          ; Color 3
    sw $t1, GP0($a0)
    li $t1, 0x00EF0000          ; Vertex 3
    sw $t1, GP0($a0)

    ; -------------------------------------------------------------------------
    ; Draw a flat-shaded triangle using a subroutine with arguments passed through the stack
    ; -------------------------------------------------------------------------
    la $sp, 0x00103CF0        ; Initialize stack pointer
    addiu $sp, -(4 * 7)       ; Subtract from stack pointer since we're about to push 7 words to memory
    li $t0, 0xFF4472          ; Param: color
    sw $t0, 0($sp)
    li $t0, 200               ; Param: x1
    sw $t0, 4($sp)
    li $t0, 40                ; Param: y1
    sw $t0, 8($sp)
    li $t0, 288               ; Param: x2
    sw $t0, 12($sp)
    li $t0, 56                ; Param: y2
    sw $t0, 16($sp)
    li $t0, 224               ; Param: x3
    sw $t0, 20($sp)
    li $t0, 200               ; Param: y3
    sw $t0, 24($sp)
    jal DrawFlatTriangle
    nop

LoopForever:
    j LoopForever                ; Continuous loop
    nop

; -----------------------------------------------------------------------------
; Subroutine to draw a flat-shaded triangle using the stack to pass arguments
; Args:
; $a0    = IO_BASE_ADDR (IO ports at 0x1F90****)
; $sp+0  = Color (for example: 0xBBGGRR)
; $sp+4  = x1
; $sp+8  = y1
; $sp+12 = x2
; $sp+16 = y2
; $sp+20 = x4
; $sp+24 = y3
; -----------------------------------------------------------------------------
DrawFlatTriangle:
    lui  $t0, 0x2000
    lw   $t1, 0($sp)
    nop
    or   $t8, $t0, $t1
    sw   $t8, GP0($a0)
    lw   $t1, 4($sp)
    lw   $t2, 8($sp)
    nop
    sll  $t2, $t2, 16
    andi $t1, $t1, 0xFFFF
    or   $t8, $t1, $t2
    sw   $t8, GP0($a0)
    lw   $t1, 12($sp)
    lw   $t2, 16($sp)
    nop
    sll  $t2, $t2, 16
    andi $t1, $t1, 0xFFFF
    or   $t8, $t1, $t2
    sw   $t8, GP0($a0)
    lw   $t1, 20($sp)
    lw   $t2, 24($sp)
    nop
    sll  $t2, $t2, 16
    andi $t1, $t1, 0xFFFF
    or   $t8, $t1, $t2
    sw   $t8, GP0($a0)
    addiu $sp, $sp, (4 * 7)
    jr $ra
    nop

End:

.close