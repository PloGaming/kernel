#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "kernel.h"
#include "io/io.h"

struct idt_desc idt_descriptors[LEOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

void int21h_handler(){
    print("Keyboard pressed\n");
    outb(0x20, 0x20); //Send an acknoledgment
}

void no_interrupt_handler(){
    outb(0x20, 0x20); //Send an acknoledgment
}

void idt_zero() {
    print("Divide by zero error\n");
}

// Given an interrupt number, this function will load an interrupt to the idtr
void idt_set(int interrupt_number, void *address){
    struct idt_desc *desc = &idt_descriptors[interrupt_number]; 
    desc->offset_1 = (uint32_t) address & 0x0000ffff; 
    desc->selector = KERNEL_CODE_SELECTOR ;
    desc->zero = 0x00;
    desc->type_attr = 0xEE; //Binary 11101110, perfect combination for setting the flags   
    desc->offset_2 = (uint32_t) address >> 16;  // Because we want the higher 8 bits
}

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1; // Setting the length of the descriptors
    idtr_descriptor.base = (uint32_t) idt_descriptors; // Setting the starting address of the descriptor

    for(int i = 0; i < LEOS_TOTAL_INTERRUPTS; i++){
        idt_set(i, no_interrupt); // Setting up the interrupts for all the cases
    }

    idt_set(0, idt_zero); //Division by zero
    idt_set(0x20, int21h); //Keyboard pressed
    

    // Load the idt from assembly function

    idt_load(&idtr_descriptor);
}