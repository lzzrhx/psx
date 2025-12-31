.psx
.create "joypad.bin", 0x80010000

.org 0x80010000

; ---------------------
; IO Port
; ---------------------
IO_BASE_ADDR equ 0x1F80      ; IO Ports Memory map base address

; ---------------------
; GPU Registers
; ---------------------
GP0 equ 0x1810               ; GP0 @ $1F801810: Rendering data & VRAM Access
GP1 equ 0x1814               ; GP1 @ $1F801814: Display Control & Environment setup

; ---------------------
; Joypad Constants
; ---------------------
PAD_L2     equ 0x0001        ; Joypad Input: L2       (Bit 0)
PAD_R2     equ 0x0002        ; Joypad Input: R2       (Bit 1)
PAD_L1     equ 0x0004        ; Joypad Input: L1       (Bit 2)
PAD_R1     equ 0x0008        ; Joypad Input: R1       (Bit 3)
PAD_T      equ 0x0010        ; Joypad Input: Triangle (Bit 4)
PAD_C      equ 0x0020        ; Joypad Input: Circle   (Bit 5)
PAD_X      equ 0x0040        ; Joypad Input: X        (Bit 6)
PAD_S      equ 0x0080        ; Joypad Input: Square   (Bit 7)
PAD_SELECT equ 0x0100        ; Joypad Input: Select   (Bit 8)
PAD_L3     equ 0x0200        ; Joypad Input: L3       (Bit 9)  (Analog Mode Only)
PAD_R3     equ 0x0400        ; Joypad Input: R3       (Bit 10) (Analog Mode Only)
PAD_START  equ 0x0800        ; Joypad Input: Start    (Bit 11)
PAD_UP     equ 0x1000        ; Joypad Input: Up       (Bit 12)
PAD_RIGHT  equ 0x2000        ; Joypad Input: Right    (Bit 13)
PAD_DOWN   equ 0x4000        ; Joypad Input: Down     (Bit 14)
PAD_LEFT   equ 0x8000        ; Joypad Input: Left     (Bit 15)

; ---------------------
; Joypad Buffer & Data
; ---------------------
PadBuffer: .word 0           ; Pad Buffer (automatically stored every frame)
PadData:   .word 0           ; Pad Data (read from VSync routine)

; ---------------------
; X,Y GLobal Variables
; ---------------------
XPos: .word 0                ; Variable to hold the position top-left X of the rectangle
YPos: .word 0                ; Variable to hold the position top-left Y of the rectangle

; ---------------------
; Main code entry point
; ---------------------
Main:
  ; ---------------------------------------------------------------------------
  ;; Initialize pad via a BIOS routine
  ; ---------------------------------------------------------------------------
  li $t1, 0x15
  li $a0, 0x20000001
  li $t2, 0xB0
  la $a1, PadBuffer          ; Set PadBuffer to automatically update each frame
  jalr $t2                   ; Jump to BIOS function
  nop

  ; ---------------------------------------------------------------------------
  ; Set $a0 as the global parameter with the IO_BASE_ADDR to be used by subs
  ; ---------------------------------------------------------------------------
  lui $a0, IO_BASE_ADDR      ; Global Param: I/O Port Base Address (0x1F80****)

  ; ---------------------------------------------------------------------------
  ; Send commands to GP1 (mapped at 0x1F801814)
  ; These GP1 is for display control and environment setup
  ; (Command = 8-Bit MSB, Parameter = 24-Bit LSB)
  ; CCPPPPPP: CC=Command PPPPPP=Parameter
  ; ---------------------------------------------------------------------------
  li $t1, 0x00000000         ; 00 = Reset GPU
  sw $t1, GP1($a0)           ; Write to GP1

  li $t1, 0x03000000         ; 03 = Display enable
  sw $t1, GP1($a0)           ; Write to GP1

  li $t1, 0x08000001         ; 08 = Display mode (320x240, 15-bit, NTSC)
  sw $t1, GP1($a0)           ; Write to GP1

  li $t1, 0x06C60260         ; 06 = Horz Display Range - 0bxxxxxxxxxxXXXXXXXXXX (3168..608)
  sw $t1, GP1($a0)           ; Write to GP1
  
  li $t1, 0x07042018         ; 07 = Vert Display Range - 0byyyyyyyyyyYYYYYYYYYY (264..24)
  sw $t1, GP1($a0)           ; Write to GP1

  ; ---------------------------------------------------------------------------
  ; Send commands to GP0 (mapped at 0x1F801810)
  ; These GP0 commands are to setup the drawing area
  ; (Command = 8-Bit MSB, Parameter = 24-Bit LSB)
  ; CCPPPPPP  CC=Command PPPPPP=Parameter
  ; ---------------------------------------------------------------------------
  li $t1, 0xE1000400         ; E1 = Draw Mode Settings
  sw $t1, GP0($a0)			     ; Write to GP0

  li $t1, 0xE3000000		     ; E3 = Drawing Area TopLeft - 0bYYYYYYYYYYXXXXXXXXXX (10 bits for Y and X)
  sw $t1, GP0($a0)	         ; Write to GP0
  
  li $t1, 0xE403BD3F         ; E4 = Drawing area BottomRight - 0bYYYYYYYYYYXXXXXXXXXX (10 bits for X=319 and Y=239)
  sw $t1, GP0($a0)           ; Write to GP0

  li $t1, 0xE5000000         ; E5 = Drawing Offset - 0bYYYYYYYYYYYXXXXXXXXXXXX (X=0, Y=0)
  sw $t1, GP0($a0)		       ; Write to GP0

Refresh:

; ---------------------------------------------------------------------------
;; Wait VSync & Store XOR PadData
; ---------------------------------------------------------------------------
WaitVSync:
  la $a1, PadBuffer          ; Load PadBuffer Address
Wait:                        ; Wait For VSync & store XOR Pad Data
  lw $t0, 0($a1)             ; Load PadBuffer
  nop                        ; Delay Slot
  beqz $t0, Wait             ; if (Pad Buffer == 0), we keep waiting...
  nor $t0, $r0               ; NOR Compliment Pad Data Bits (Delay Slot) - happens before the above branch
  sw $r0, 0($a1)             ; Store Zero To PadBuffer
  la $a1, PadData            ; Load PadData Address
  sw $t0, 0($a1)             ; Store PadData

; -----------------------------------------------------------------------------
;; Check Joypad buttons that are currently being pressed
; -----------------------------------------------------------------------------
PressUp:
  la $a1, PadData            ; Load Input Data Address
  lw $t0, 0($a1)             ; Load Input Data Word
  nop                        ; Delay Slot
  andi $t0, PAD_UP           ; $t0 = Input Status
  beqz $t0, PressDown        ; If button-up is not pressed, bypass and test button-down
  nop
  ; --> Here goes logic when button Up is pressed:
  la $t2, YPos
  lw $t3, 0($t2)
  nop
  addiu $t3, $t3, -1         ; YPos--
  sw $t3, 0($t2)

PressDown:
  la $a1, PadData            ; Load Input Data Address
  lw $t0, 0($a1)             ; Load Input Data Word
  nop                        ; Delay Slot
  andi $t0, PAD_DOWN         ; $t0 = Input Status
  beqz $t0, PressRight       ; If button-down is not pressed, bypass and test button-right
  nop
  ; --> Here goes logic when button Down is pressed:
  la $t2, YPos
  lw $t3, 0($t2)
  nop
  addiu $t3, $t3, 1         ; YPos++
  sw $t3, 0($t2)

PressRight:
  la $a1, PadData            ; Load Input Data Address
  lw $t0, 0($a1)             ; Load Input Data Word
  nop                        ; Delay Slot
  andi $t0, PAD_RIGHT        ; $t0 = Input Status
  beqz $t0, PressLeft        ; If button-right is not pressed, bypass and test button-left
  nop
  ; --> Here goes logic when button Right is pressed:
  la $t2, XPos
  lw $t3, 0($t2)
  nop
  addiu $t3, $t3, 1          ; XPos++
  sw $t3, 0($t2)

PressLeft:
  la $a1, PadData            ; Load Input Data Address
  lw $t0, 0($a1)             ; Load Input Data Word
  nop                        ; Delay Slot
  andi $t0, PAD_LEFT         ; $t0 = Input Status
  beqz $t0, EndInputCheck    ; If button-left is not pressed, bypass and end input check
  nop
  ; --> Here goes logic when button Left is pressed:
  la $t2, XPos
  lw $t3, 0($t2)
  nop
  addiu $t3, $t3, -1         ; XPos--
  sw $t3, 0($t2)

EndInputCheck:

; -----------------------------------------------------------------------------
; Clear the screen (draw a rectangle on VRAM).
; -----------------------------------------------------------------------------
ClearScreen:
  li $t1, 0x02422E1B         ; 02 = Fill Rectancle in VRAM (Parameter Color: 0xBBGGRR)
  sw $t1, GP0($a0)           ; Write GP0 Command
  li $t1, 0x00000000         ; Fill Area, Parameter: 0xYYYYXXXX - Topleft (0,0)
  sw $t1, GP0($a0)           ; Write to GP0
  li $t1, 0x00EF013F         ; Fill Area, 0xHHHHWWWW (Height=239, Width=319)
  sw $t1, GP0($a0)           ; Write to GP0

; -----------------------------------------------------------------------------
; Draw a small rectangle at position (XPos,YPos)
; -----------------------------------------------------------------------------
DrawRect:
  li $t1, 0x0200FF00         ; 02 = Fill Rectancle in VRAM (Parameter Color: 0xBBGGRR)
  sw $t1, GP0($a0)           ; Write GP0 Command

  la   $t2, YPos
  lw   $t3, 0($t2)
  nop
  sll  $t3, $t3, 16           ; YPos <<= 16
  la   $t2, XPos
  lw   $t4, 0($t2)
  nop
  andi $t4, $t4, 0xFFFF      ; XPos &= 0xFFFF
  or   $t5, $t3, $t4         ; YPos | Xpos
  sw   $t5, GP0($a0)         ; Write to GP0 (YYYYXXXX)

  li $t1, 0x00200020         ; Fill Area, 0xHHHHWWWW (Height=32, Width=32)
  sw $t1, GP0($a0)           ; Write to GP0

  j Refresh                  ; Continuous refresh loop
  nop

.close
