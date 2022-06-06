#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define Debug 1

int main(int argc,char *argv[]){

char file[20];
int CID = atoi(argv[1]); // get CID from command line 
printf("CID: %d ", CID);

printf("\nEnter input file name: ");
scanf("%s" , file); // get file name from user 

FILE * fp; int M, i; 
      fp = fopen(file, "r");
      fscanf(fp, "%d" , &M);
      printf("\n");
      printf("Total number: %d\n", M);
    
      int array[M]; // create array for the file

    /*for (i =0; i< M ;i++){
        array[i] = 0;
        printf("%d\n" ,array[i]);

    }*/
    //read the array and store the array in array[M]
    for (i= 0; i < M; i++){
        fscanf(fp, "%d" ,&array[i]);
         printf("%d " , array[i]);   
    }

char *ip = "127.0.0.1";
int port = 5566; //last 4 digits of student ID 

int sock;
struct sockaddr_in addr;
socklen_t addr_size;
char buffer[1024];
int n;

sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
    perror ("[-]Socket error");
    exit(1);
}
printf("\n");
printf ("\n[+]TCP client socket created.\n");

memset(&addr, '0', sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = port;
addr.sin_addr.s_addr = inet_addr(ip); 

connect (sock, (struct sockaddr*)&addr, sizeof(addr));
printf("\nConnected to the server.\n");

send(sock, &CID, sizeof(int),0); // send cid 
printf("Sent CID :%d", CID );

bzero (buffer, 1024);
strcpy(buffer, file);
printf("\nSent file name: %s\n", buffer);
send(sock, buffer, strlen(buffer)+1,0); // send file name 

int sendSize = sizeof(array)/4;
printf("\n");
printf("Sent Array size :%d", sendSize );

// send the array size first 
int r; // to hold return value of send 
r=send(sock, &sendSize, sizeof(int),0);
if (r!=4)
{printf("failed to send array size: %d",sendSize);
    //send(sock, &sendSize, sizeof(int),0);
    }
if (Debug)
printf("\n[Debug] send func return value:%d" ,r);

// send the array
r=send(sock, array, sizeof(array),0);
if (r!=4*sendSize)
{printf("failed to send array! only sent %d items, need to send %d items" ,r/4, sendSize);
    send(sock, &sendSize, sizeof(int),0);}
printf("\nArray sent to server!\n"); //send array
if (Debug)
printf("[Debug] send func return value:%d" ,r);

//memcpy(&buffer,array,sendSize);
//strcpy(buffer, (char*)&array);
//printf("\nClient: %s\n", buffer);
//send(sock, (char*)array, sendSize,0);
//printf("\nClient: %s\n", buffer);
//send(sock, buffer, strlen(buffer),0);
//send(sock, buffer, sendSize,0);
bzero(buffer, 1024);
recv(sock, buffer, sizeof(buffer),0);
printf("\nMessage from Server: %s\n" , buffer);

close(sock);
printf("Disconnected from the server!\n") ;

return 0; 
}