section .asm ; Assembly section

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; points to the first argument. + 4 points to the return address
    lidt [ebx] ; loads the interrupt descriptor table

    pop ebp
    ret

int21h:
    cli;
    pushad
    
    call int21h_handler
    
    popad
    sti;
    iret

no_interrupt:
    cli;
    pushad
    
    call no_interrupt_handler
    
    popad
    sti;
    iret