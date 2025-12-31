.psx
.create "image.bin",0x80010000

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

; -----------------------------------------------------------------------------
; Image constants
; -----------------------------------------------------------------------------
IMG_WIDTH equ 640
IMG_HEIGHT equ 480
IMG_SIZE_BYTES equ 921600       ; 640 * 480 * 3

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
    li $t1, 0x08000037          ; Command 08 = Display mode (640x480, 24-bit, NTSC)
    sw $t1, GP1($a0)            ; Write to GP1
    li $t1, 0x06C60260          ; Command 06 = Horizontal display range 0xXXXxxx (3168-608)
    sw $t1, GP1($a0)            ; Write to GP1
    li $t1, 0x0707E018          ; Command 07 = Vertical display range %yyyyyyyyyyYYYYYYYYYY (504-24)
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

    ; ---------------------------------------------------------------------------
    ; Copy a rectangle from CPU to VRAM
    ; ---------------------------------------------------------------------------
    li $t1, 0xA0000000         ; A0 = Copy Rect CPU to VRAM (0xA0000000)
    sw $t1, GP0($a0)           ; Write GP0 Command
    li $t1, 0x00000000         ; Copy Area, TopLeft (x=0, y=0)
    sw $t1, GP0($a0)           ; Write to GP0
    li $t1, 0x01E003C0         ; Copy Area, 0xHHHHWWWW (Height=480, Width=640)
    sw $t1, GP0($a0)           ; Write to GP0
    li $t0, IMG_SIZE_BYTES   ; Load the total of bytes of the image
    srl $t0, 2                 ; $t0 / 4
    la $t2, Image             ; $t2 = image offset in memory
LoopWords:
    lw    $t1, 0($t2)          ; t1 = word from the image array
    addiu $t2, 4               ; $t2 += 4 (delay slot)
    sw    $t1, GP0($a0)        ; Write to GP0
    bnez  $t0, LoopWords       ; if ($t0 != 0), keep looping
    addiu $t0, $t0, -1         ; t0-- (delay slot)

End:

LoopForever:
    j LoopForever                ; Continuous loop
    nop

Image:
    .incbin "logo.bin"           ; Include a 640x480*3 (total of 921,600 bytes)

.close