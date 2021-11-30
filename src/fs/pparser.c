#include "pparser.h"
#include "string/string.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"

//Function that verify if the path is in a valid format
static int pathparser_path_valid_format(const char* filename)
{
    int len = strnlen(filename, LEOS_MAX_PATH);
    return (len >= 3 && isDigit(filename[0]) && memcmp((void*)&filename[1], ":/", 2) == 0);
}

//Function for returning the drive number
static int pathparser_get_drive_by_path(const char** path)
{   
    //Checking if the given path is correct, if not return error
    if(!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }

    int drive_no = to_numeric_digit(*path[0]);

    // Add 3 bytes for skip the drive number 0:/
    *path += 3;
    return drive_no;
}

//Function for creating the starting prototype of out path
static struct path_root* pathparser_create_root(int drive_number)
{   
    //Allocating it in memory so it persists
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}

//Function for returning the next part of the path
static const char* pathparser_get_path_part(const char **path)
{
    char* result_path_part = kzalloc(LEOS_MAX_PATH);
    int i = 0;
    while(**path != '/' && **path != 0x00)
    {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if(**path == '/')
    {
        //Skip the forward slash to avoid problems
        *path += 1;
    }

    if(i == 0)
    {
        kfree(result_path_part);
        result_path_part = 0;
    }
    
    return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path)
{
    const char* path_part_str = pathparser_get_path_part(path);
    if(!path_part_str)
    {
        return 0;
    }

    //Creating a new path_part and assigning the names
    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;

    //Linking the last part with the new one
    if(last_part)
    {
        last_part->next = part;
    }

    return part;
}


void pathparser_free(struct path_root* root)
{
    struct path_part* part = root->first;
    while(part)
    {
        struct path_part* next_part = part->next;
        kfree((void*) part->part);
        kfree(part);
        part = next_part;
    }

    kfree(root);
}

//Function for parsing a given path of a form discNumber:/dir1/.../
struct path_root* pathparser_parse(const char* path, const char* current_directory_path)
{
    int res = 0;
    const char* tmp_path = path;
    struct path_root* path_root = 0;

    //Check if the len of the string is too high
    if(strlen(path) > LEOS_MAX_PATH)
    {
        goto out;
    }

    res = pathparser_get_drive_by_path(&tmp_path);
    if(res < 0)
    {
        goto out;
    }
    //Now the tmp_path points 3 bytes after, so it skips 0:/

    //This will create the initial setup of out path
    path_root = pathparser_create_root(res);
    if(!path_root)
    {
        goto out;
    }

    //Creating the first part
    struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if(!first_part)
    {
        goto out;
    }

    //Linking the first part to our path_root
    path_root->first = first_part;

    //While there is a part, we continue
    struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);
    while(part)
    {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

    
out:
    return path_root;
}