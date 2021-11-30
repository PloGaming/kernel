#include "string.h"

//Function for retrieving the length of a string
int strlen(const char* str)
{

    int len = 0;
    while(str[len]){
        len++;
    }

    return len;
}

//Function for checking if a given character is a number
bool isDigit(char c)
{
    return c >= 48 && c <= 57;
}

//Function for taking an ASCII digit and convert to an integer
int to_numeric_digit(char c)
{
    return (int) c - 48;
}

//Function for retrieving the length of a string of a maximum number of characters
int strnlen(const char* str, int max)
{

    int i = 0;
    for(i = 0; i < max; i++)
    {
        if(str[i] == 0)
        {
            break;
        }
    }
    return i;
}