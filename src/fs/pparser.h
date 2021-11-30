#ifndef PATHPARSER_H
#define PATHPARSER_H
#include "kernel.h"

struct path_root
{
    int drive_no;
    struct path_part* first;
};

struct path_part
{
    const char* part;
    struct path_part* next;
};

struct path_root* pathparser_parse(const char* path, const char* current_directory_path);
void pathparser_free(struct path_root* root);

//Example 0:/test/abc.txt
//This is a path root
//It contains a drive number (0)
//And a path part (test)
//The part is test
//the path part is a pointer to the next element (abc.txt)

#endif