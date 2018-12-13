#ifndef COMMON
#define COMMON
    
#include <stddef.h>    

#define  WAITING_TIME 1000000;
#define  WAITING_TIME_SEC 0

typedef struct 
{
    char filename[1000];
    char domain[1000];
    char connection[1000];
    size_t size;
    size_t content_start;
    char *buffer;
} HTTP_r ;
    
#endif