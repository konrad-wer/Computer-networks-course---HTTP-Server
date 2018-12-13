
server : makefile main.o write_HTTP.o read_HTTP.o 
	gcc -o server main.o write_HTTP.o read_HTTP.o -std=c99 -Wall -Wextra 
	
main.o : makefile main.c write_HTTP.o read_HTTP.o common.h
	gcc -c -o main.o main.c -std=c99 -Wall -Wextra 

write_HTTP.o : makefile write_HTTP.h write_HTTP.c common.h
	gcc -c -o write_HTTP.o write_HTTP.c -std=c99 -Wall -Wextra 

read_HTTP.o : makefile read_HTTP.h  read_HTTP.c common.h
	gcc -c -o read_HTTP.o read_HTTP.c -std=c99 -Wall -Wextra 

clean :
	rm *.o

distclean :
	rm *.o
	rm server 
