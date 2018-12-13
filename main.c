//Konrad Werbliński 291878

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h> 
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include "read_HTTP.h"
#include "write_HTTP.h"
#include "common.h"


int is_natutal_number(char *c)
{   
    do
    {
        if(!isdigit(*c))
            return 0;
    }
    while(*(++c));
    return 1;
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        puts("USAGE: ./server port direcory");
        return 0;
    }
    
    
    if(!is_natutal_number(argv[1]))
    {
        puts("Port should be natural number!");
        return 0;
    }
    
    char *dir = argv[2];
    
    struct stat filestat; 
    if(stat(dir, &filestat) || !S_ISDIR(filestat.st_mode))
    {
        puts("Not a directory or a directory doesn't exists!");
        return 0;
    }
    
    int port = atoi(argv[1]);
    
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
    {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
	if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)  
    {
		fprintf(stderr, "bind error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}
    
    if (listen(sockfd, 64) < 0) 
    {
        fprintf(stderr, "listen error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
    }
    
    while(1)
    {
        struct sockaddr_in client_address;
        socklen_t len = sizeof(client_address);
        int conn_sockfd = accept (sockfd, (struct sockaddr*)&client_address, &len);
        
        if (conn_sockfd < 0)
        {
            fprintf(stderr, "accept error: %s\n", strerror(errno)); 
            return EXIT_FAILURE;
        }
        char ip_address[20];
        inet_ntop (AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));
        printf ("New client %s:%d\n", ip_address, ntohs(client_address.sin_port)); 
        
        
        fd_set descriptors;
        FD_ZERO (&descriptors);
        FD_SET (conn_sockfd, &descriptors);
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = WAITING_TIME;
        int keep_alive = 1; 
        fcntl(conn_sockfd, F_SETFL, O_NONBLOCK);
        
        while(keep_alive)
        {       
            keep_alive = select(conn_sockfd + 1, &descriptors, NULL, NULL, &tv);
            if(keep_alive == -1)
            {
                fprintf(stderr, "select error: %s\n", strerror(errno)); 
                return EXIT_FAILURE;
            }    
            
            char buffer[IP_MAXPACKET];
            int bytes_recieved;
            while ((bytes_recieved = recv (conn_sockfd, buffer, IP_MAXPACKET, 0)) > 0)
            {                
                buffer[bytes_recieved] = 0;
                HTTP_r h = read_HTTP(buffer);
                h.content_start = 0;
                
                if(strcmp(h.connection, "close") == 0)
                    keep_alive = 0;
                else 
                {
                    keep_alive = 1;
                    tv.tv_usec = WAITING_TIME;
                    tv.tv_sec  = WAITING_TIME_SEC;
                }

                make_response(&h, dir);
                
                if(send(conn_sockfd, h.buffer, h.size, 0) < (long long)h.size)
                {
                    fprintf(stderr, "send error: %s\n", strerror(errno)); 
                    return EXIT_FAILURE;
                }    
             
                free(h.buffer);
            }
            
            if(!bytes_recieved)
                break;
        }
        
        if (close(conn_sockfd) < 0)
        {
            fprintf(stderr, "close error: %s\n", strerror(errno)); 
            return EXIT_FAILURE;
        }
    }
 
    
    
    close(sockfd);
}