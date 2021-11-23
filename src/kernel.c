#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

uint16_t *video_mem = 0;
uint16_t terminal_col = 0;
uint16_t terminal_row = 0;

//Function for retrieving the length of a string
size_t strlen(const char* str)
{

    size_t len = 0;
    while(str[len]){
        len++;
    }

    return len;
}

//Function for converting a character to an hex
uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c; //So when we overwritten with the c character (it has to be reverse because of the endianess).
}

//Function for outputting a character to the screen
void terminal_putchar(int x, int y, char c, char colour )
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour); //Convert an x and y coordinate to an index
}

//Function for appending a character to the screen
void terminal_write_char(char c, char colour)
{
    if(c == '\n'){
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if(terminal_col >= VGA_WIDTH){
        terminal_col = 0;
        terminal_row += 1;
    }
}

//Function for cleraing the screen
void terminal_initialize()
{
    terminal_col = 0;
    terminal_row = 0;
    video_mem = (uint16_t *) (0xB8000);
    for(int y = 0; y < VGA_HEIGHT; y++)
    {
        for(int x = 0; x < VGA_WIDTH; x++){
            terminal_putchar(x, y, ' ', 0); 
        }
    }
}

void print(const char *str){
    size_t len = strlen(str);
    for(int i = 0; i < len; i++){
        terminal_write_char(str[i], 15);
    }
}


//The main function of the kernel
void kernel_main()
{
    terminal_initialize();
    print("Avvio LeoOs in corso ... \n");
    print("Inizializzazione interrupt descriptor table (idt) ... \n");

    //Initialize the heap;
    kheap_init();

    // Initialize the idt
    idt_init();

}