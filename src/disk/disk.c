//Documentation https://wiki.osdev.org/ATA_read/write_sectors
//Section Read in LBA mode

#include "io/io.h"

//Function for reading a sector from a disk, and storing it in a buffer
int disk_read_sector(int lba, int total, void* buf)
{
    //Sending the lba to the port
    outb(0x1F6, ( lba >> 24)| 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    //A pointer that points to the start of our buffer
    unsigned short* ptr = (unsigned short*) buf;
    
    for(int i = 0; i < total; i++){
        
        //Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        
        {
            c = insb(0x1F7);
        }

        //Now we're ready 
        //Copy from hard disk to memory
        for(int i = 0; i < 256; i++)
        {
            //Inserting 2 bytes (1 word) at a time in the buffer
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}
