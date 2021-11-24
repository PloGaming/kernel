[BITS 32]

global _start ; publicly exporting the symbol

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    ; Setting up the segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable the A20 line for the last bit
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap the master PIC (Programmable interrupt controller)
    mov al, 00010001b
    out 0x20, al ; Set the PIC to an initialization mode
    
    mov al, 0x20 ; This is where master ISR should start
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ; End remap of the master PIC

    call kernel_main
    
    jmp $

times 512 - ( $ - $$ ) db 0  