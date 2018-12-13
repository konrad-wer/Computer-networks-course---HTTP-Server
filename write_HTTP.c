//Konrad Werbliński 291878

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "common.h"

char *get_filetype(char *filename)
{
    char end[1000];
    end[0] = 0;
    int dot_at = -1;
    
    for(int i = 0; filename[i]; i++)
        if(filename[i] == '.')
            dot_at = i;
        
        
    if(dot_at >= 0)
    {
        int i;
        for (i = 0; filename[i + dot_at]; i++)
            end[i] = filename[i + dot_at];
    
        end[i] = 0;
    }
    
    if(strcmp(end, ".txt") == 0)
        return "text/plain";
    else if(strcmp(end, ".html") == 0)
        return "text/html";
    else if(strcmp(end, ".css") == 0)
        return "text/css";  
    else if(strcmp(end, ".jpg") == 0)
        return "image/jpg";
    else if(strcmp(end, ".jpeg") == 0)
        return "image/jpeg";
    else if(strcmp(end, ".png") == 0)
        return "image/png";
    else if(strcmp(end, ".pdf") == 0)
        return "application/pdf";
    else
        return "application/octet-stream";
}




void load_file(HTTP_r *h, FILE *f)
{
    char *c = h -> buffer + h -> content_start;
   
    while (!feof(f)) 
        *c++ = fgetc(f);
}



void make_response(HTTP_r *h, char *dir)
{     
    if(!h -> filename[0] && !h -> domain[0] && !h -> connection[0])
    {
        h -> buffer = malloc(256);
        sprintf(h -> buffer, 
            "HTTP/1.1 501 Not Implemented\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 101\r\n"
            "\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<body>\n"
                "<h1>501 - Not Implemented</h1>\n"
                "<p>You are a teapot!</p>\n"
            "</body>\n"
            "</html>");
        h -> size = strlen(h -> buffer);
        return;
    }

    struct stat filestat;   
    
    int not_empty_filename = strcmp(h -> filename, "/");
    char tmp[1000]; 
    tmp[0] = 0;
    strcpy(tmp, dir);
    strcat(tmp, "/");
    strcat(tmp, h -> domain);
    strcat(tmp, h-> filename);
    strcpy(h -> filename, tmp);  
    
    if(strstr(h -> filename, ".."))
    {
        h -> buffer = malloc(256);
        sprintf(h -> buffer, 
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 97\r\n"
            "\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<body>\n"
                "<h1>403 - Forbidden</h1>\n"
                "<p>You shall not pass!</p>\n"
            "</body>\n"
            "</html>");
        h -> size = strlen(h -> buffer);
        return;
    }
    
    
    char index[1000];
    index[0] = 0;
    strcpy(index, dir);
    strcat(index, "/");
    strcat(index, h -> domain);
    strcat(index, "/index.html");
    
    int index_html = stat(index, &filestat);
    int file_error = stat(h -> filename, &filestat);
  

    if(!file_error && not_empty_filename)
    {
        h -> buffer = malloc(100 + (long long)filestat.st_size);
        FILE *f = fopen(h -> filename, "rb");
        sprintf(h-> buffer, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %lld\r\n"
            "\r\n",
            get_filetype(h -> filename), (long long)filestat.st_size);
            
        h -> size = strlen(h -> buffer) + (long long)filestat.st_size;
        h -> content_start = strlen(h -> buffer);
        load_file(h, f);
        
        fclose(f); 
    }
    else if(!index_html && !not_empty_filename)
    {
        stat(index, &filestat);
        
        h -> buffer = malloc(100 + (long long)filestat.st_size);
        FILE *f = fopen(index, "rb");
        sprintf(h-> buffer, 
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %lld\r\n"
            "\r\n",
            (long long)filestat.st_size);
        
        h -> size = strlen(h -> buffer) + (long long)filestat.st_size;
        h -> content_start = strlen(h -> buffer);
        load_file(h, f);
        
        fclose(f); 
        
    }
    else
    {
        h -> buffer = malloc(256);
        sprintf(h -> buffer, 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 120\r\n"
            "\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<body>\n"
                "<h1>404 - Not Found</h1>\n"
                "<p>These aren't the files you're looking for.</p>\n"
            "</body>\n"
            "</html>");
        h -> size = strlen(h -> buffer);
    }
}