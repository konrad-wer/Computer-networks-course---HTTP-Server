//Konrad Werbliński 291878

#include <stdio.h>
#include <string.h>
#include "common.h"

HTTP_r read_HTTP(char *request)
{
    char line[1000];
    HTTP_r r; //Zespól R znowu błysnął
    
    r.filename[0] = r.domain[0] = r.connection[0] = 0;
     
    while(*request)
    {
        int i = 0;
        while(*request && *request != '\n')
            line[i++] = *request++;
        
        line[i] = 0;
        
        char arg[1000];
        arg[0] = 0;

        if(sscanf(line, "GET %s", arg))
            strcpy(r.filename, arg);
        if (sscanf(line, "Host: %s", arg))
            strcpy(r.domain, arg);
        if (sscanf(line, "Connection: %s", arg))
            strcpy(r.connection, arg);
              
        if(*request)
            request++;
    }
    
    for(char *c = r.domain; *c; c++)
        if(*c == ':')
            *c = 0;
    
    return r;
    
}