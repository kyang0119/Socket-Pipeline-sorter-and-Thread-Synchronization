# written by kuan yang 
# this makefile make the executable admin.exe and client.exe


all: admin.exe client.exe

admin.exe: admin.o cal-new.o 
	gcc -g -o admin.exe admin.o cal-new.o -lpthread -lm


client.exe: client.o 
	gcc -g -o client.exe client.o 


cal-new.o: cal-new.c
	gcc -g -c cal-new.c


admin.o: admin.c
	gcc -g -c admin.c


client.o: client.c 
	gcc -g -c client.c


clean: 
	rm client.o admin.o cal-new.o
