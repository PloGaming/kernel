[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    mov cr3, eax ;cr3 should contain the address to out page directory

    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp

    mov eax, cr0; Because we can't change cr0 directly
    or eax, 0x80000000; Load the 31 byte
    mov cr0, eax; Enable paging

    pop ebp
    ret