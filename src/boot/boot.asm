ORG 0x7c00
BITS 16

;Initiate the Code Segment
CODE_SEG equ gdt_code - gdt_start
;Initiate the Data Segment 
DATA_SEG equ gdt_data - gdt_start
; Setup for the bpb (bios parameter block)

_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0:step2 ; setup the code segment

step2:
    cli ; disable interrupts (sets the Interrupt flag to 0)
    ; Setup all the segment registers
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; enable interrupts (sets the Interrupt flag to 1)

.load_protected:
    cli ; We disable interrupts
    lgdt[gdt_descriptor] ; we allocate our gdt
    mov eax, cr0 ; We need to change the last bit of cr0 for entering 32 bit mode
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32
    


;GDT Global Descriptor Table

gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

gdt_code: ; CS should point to this
    dw 0xffff
    dw 0
    db 0
    db 0x9a
    db 11001111b
    db 0
gdt_data: ; DS, ES, SS, FS, GS should point to this
    dw 0xffff
    dw 0
    db 0
    db 0x92
    db 11001111b
    db 0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; The size of the descriptor
    dd gdt_start ; The offset

[BITS 32]
load32:
    mov eax, 1 ; starting sector of the kernel (0 is the boot sector)
    mov ecx, 100 ; number of sector readed
    mov edi, 0x0100000 ; The address the kernel will be
    call ata_lba_read
    jmp CODE_SEG:0x0100000 ;jumping to kernel

; This subroutine is going to read through ports our disk
ata_lba_read:
    mov ebx, eax ; Backup the LBA
    ;Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24 ; shift eax of 24 bits so it will contain the high 8 bits of the container (32 - 24 = 8)
    or eax, 0xE0 ; Select the master drive
    mov dx, 0x1F6 ; The port
    out dx, al ; Al contains the 8 bits we talked before

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Sending more bits of the LBA
    mov eax, ebx ; Restoring the lba
    mov dx, 0x1F3
    out dx, al
    ;Finished sending more bits for LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restoring the lba
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx ; REMEMBER total number of sectors

; Checking if we can read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

;We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1f0
    rep insw ; Reads a word from port specified in dx and it stores it in the address specified in edi and do it for 256 times
    ;This read 256 words or 512 bytes (1 sector)
    pop ecx
    loop .next_sector ; decrement the number of sectors
    ; End of reading sectors into memory
    ret

; out instruction permit us to communicate with the data bus of the CPU
; $ means the address of the current instruction, while $$ means the address of the first section (code), so it will pad exactly the number you need

times 510 - ($ - $$) db 0
dw 0xAA55
