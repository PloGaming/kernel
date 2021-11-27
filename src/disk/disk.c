//Documentation https://wiki.osdev.org/ATA_read/write_sectors
//Section Read in LBA mode

#include "disk.h"
#include "io/io.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"

//This will represents the primary disk
struct disk disk;

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

//This function will search and initialize every disk found
void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = LEOS_DISK_TYPE_REAL;
    disk.sector_size = LEOS_SECTOR_SIZE;
}

//Get a disk from an index
struct disk* disk_get(int index)
{
    if(index != 0){
        return 0;
    }

    return &disk;
}

//Read a block from a disk
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    if(idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}