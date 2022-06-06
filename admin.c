
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "function.h"
#include <math.h>

int main() {
/*int fd[2]; // file descriptor
if (pipe(fd)== -1){
    return 2;
}
int pid = fork(); // fork child process 
if (pid==-1){
    return 1;
}

if (pid != 0){ */// parent process get message from client , send to child 
char *ip = "127.0.0.1";
int port = 5566; // last 4 digits of student ID 

int server_sock; int client_sock;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_size;
char buffer[1024];
int n;

server_sock = socket(AF_INET, SOCK_STREAM, 0);
if (server_sock < 0) {
    perror ("[-]Socket error");
    exit(1);
}
printf("\n");
printf ("[+]TCP server socket created.\n");

memset(&server_addr,'\0', sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = port;
server_addr.sin_addr.s_addr = inet_addr(ip); 

// bind function next 

n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
if (n<0) {
    perror ("[-]Bind Error");
    exit(1);

}
printf("\n");
printf("[+]Bind to the port number: %d\n" , port);

 listen(server_sock, 5); 
 printf("\n");
 printf("Listening...\n");

 while (1) {

    int fd[2]; // file descriptor
    if (pipe(fd)== -1){ // create pipe
    return 2;
    }

    int pid = fork(); // fork child process 
    if (pid==-1){
    return 1;
    }

    if (pid != 0){

     addr_size = sizeof(client_addr);
     client_sock = accept(server_sock, (struct sockaddr*)&client_addr,&addr_size);
     printf("\n");
     printf("[+]Client connected.\n");

    int arrsize,i, CID;
    recv(client_sock, &CID, sizeof(int),0); // 1.recv cid 
    printf("\nReceived CID from client, CID: %d\n", CID);

    bzero(buffer, 1024);// clean buffer 
    recv(client_sock, buffer, sizeof(buffer)+1,0);//2. receive file name 
    printf("Received file name: %s\n" , buffer);

    char file[50];
    strcpy(file, buffer);
    //printf("file[50] content: %s\n" , file);
    
    int r ; // to hold the returned receive value
    r= recv(client_sock, &arrsize, sizeof(int),0);
    if (r!=4)
    {
     printf("failed to receive array size: %d, received array size: %d. Please run again!!",arrsize,r/4);
     close(client_sock);
     printf("[+]Client disconnected. \n");
     exit(EXIT_SUCCESS);

    }

    //recv(client_sock, &arrsize, sizeof(int),0);//3.receive array size 
    printf("\nReceived array size from client, array size: %d\n", arrsize);
    //recv(client_sock, buffer, sizeof(buffer),0);
    //recv(client_sock, buffer, sizeof(buffer),0);
    //printf("Client: %d\n" , array);
    //printf("Client: %s\n" , buffer);
    
    int arr[arrsize]; 
    recv(client_sock, arr, sizeof(arr),0); //4.receive array
    printf("\nReceived array from client:\n");
    printf("\n");
    for (i = 0; i < arrsize; i++)
    {
        printf("%d ", arr[i]);
    }

    int pbuff[arrsize]; 
    //printf("\npbuff created!\n");
    for (i = 0 ; i < arrsize; i++){
       pbuff[i] = arr[i];
       //printf("%d ",pbuff[i]);
      }

    bzero(buffer, 1024);
    strcpy(buffer, "Hi, client, this is server, I have received your message! I am going to send the info to child process to sort.\n");
    printf("\n");
    printf("\nServer: %s\n", buffer);
    send (client_sock, buffer, strlen(buffer), 0); 

    close(client_sock);
    printf("[+]Client disconnected. \n");

    // now use pipe send to child 
    close(fd[0]); // close read end of the pipe
    
    int n = arrsize;
    //printf("\nn:%d\n",n);
    
    //1. send CID through pipe
    if (write(fd[1], &CID, sizeof(int)) < 0) {
        return 3;}

    //2. send file len 
    int fileLen = strlen(file)+1;
    if (write(fd[1], &fileLen, sizeof(int)) < 0) {
        return 4;}

    //3 . send file name
    if (write(fd[1], file, strlen(file)+1) < 0) {
        return 5;}
    
    // 4. send array size
    if (write(fd[1], &n, sizeof(int)) < 0) {
        return 6;
    }
    // 5. send array 
    for (i=0; i < n; i++){
       if (write(fd[1] , &pbuff[i], sizeof(int)) < 0)
            return 7;
    }
    //if (write(fd[1], pbuff, sizeof(int)*n)<0) {
      //  return 4;
    //} 
    //close (fd[1]);
    printf("\n");
    printf("parent process sent array to child process to sort...\n");
    printf("array sent: \n");
    wait(NULL);
 }
//return 0; 

 else {
    //child process receive array
    int i; 
    close(fd[1]);
    int n, CID, fileLen; char file[50];
    
    //1. Pipe receive CID 
    if (read(fd[0], &CID, sizeof(int))< 0)
        return 8;

    //2. pipe receive file len
     if (read(fd[0], &fileLen, sizeof(int))< 0)
        return 9;
    //3. pipe receive file name
    for (i=0; i < fileLen; i++){
       if (read(fd[0] , &file[i], sizeof(char)) < 0)
            return 4;
    }
    //4. pipe receive the array size 
    if (read(fd[0], &n, sizeof(int))< 0)
        return 5;

    //printf("Child process received n: %d\n", n);
    
    int arr[n];
    //memset(arr,0,n);
    //printf("array[n] before receiving data: ");
    //for (i = 0; i < n; i++){
    //    arr[i]=0;
    //    printf("%d ", arr[i]);
    //}
    for (i=0; i < n; i++){
       if (read(fd[0] , &arr[i], sizeof(int)) < 0)
            return 4;
    }

    //if (read(fd[0],arr,sizeof(int)*n >=0))
    //    printf("\nsend array success") ;
    
    printf("\nChild process Received info from parent:\n");
    printf("====================CID=%d, file=%s, N=%d\n", CID, file,n);

    for (i = 0; i < n; i++){
         if(i != 0 && (i% 10) == 0)
            printf("\n");
        printf("%d ", arr[i]);
    }

    printf("\n");
    close (fd[0]);

    //starting to call functions from cal.c
    //printArray(arr, 0, n); // 

    task_data sortarr;
    sortarr.arr = &arr[0];
    sortarr.low = 0;
    sortarr.high = n-1;
    sortarr.size = n;
    sortarr.layer = 2;

    sorter(&sortarr);
    int M =4; // this is thread number
    int l = log(M)/log(2);// change base to get layer = log of M on base of 2
    //printf("[STAT](%s -> %s): M = %d, log(M) = %d, merge layers needed = %d\n", __FILE__, __FUNCTION__, M, l, l);
    //int i = 0;
    for(i = 0; i < l; i++) {
        sortarr.layer = i + 3;
        merger(&sortarr);
    }
    return 0;
    
    /*
    merge_sort(arr, 0, n-1);
    printf("\n");
    printf("\nSorted array: \n");
    for (i = 0; i<n; i++){
        printf("%d " , arr[i]); }
        printf("\n");
        return 0;
        */
    
} 

} 
}
 






