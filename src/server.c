#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

struct USER{
    char username[20];
    int socket_ID;
    struct sockaddr_un userAddress;
    int len;
    int status;
};

struct USER users[10];
pthread_t userthreads[10];
volatile int quitServerFlag=0;
int NUM_USERS=0;

void * execServerLoop(){
    int SERVER_SOCKET_DESCRIPTOR, RECIEVE_DESCRIPTOR;
    char toRecieve[1024];
    struct sockaddr_un SERVER_SOCKET;
    if((SERVER_SOCKET_DESCRIPTOR=socket(AF_UNIX, SOCK_STREAM, 0))==-1){
        perror("Socket init");
        exit(1);
    }
    SERVER_SOCKET.sun_family=AF_UNIX;
    sprintf(SERVER_SOCKET.sun_path, "../socket/socket_%d",getpid());
    unlink(SERVER_SOCKET.sun_path);
    if(bind(SERVER_SOCKET_DESCRIPTOR, (struct sockaddr *)&SERVER_SOCKET, strlen(SERVER_SOCKET.sun_path)+sizeof(SERVER_SOCKET.sun_family))==-1){
        perror("Socket binding");
        exit(1);
    }
    if(listen(SERVER_SOCKET_DESCRIPTOR, 1024)==-1){
        perror("Socket listening");
        exit(1);
    }
    printf("Server listening at socket_%d\n",getpid());
    while(quitServerFlag==0){
        users[NUM_USERS].socket_ID=accept(SERVER_SOCKET_DESCRIPTOR, (struct sockaddr *)&users[NUM_USERS].userAddress, &users[NUM_USERS].len);
        RECIEVE_DESCRIPTOR=recv(users[NUM_USERS].socket_ID, toRecieve, 200, 0);
        if(RECIEVE_DESCRIPTOR<0){
            perror("Recieve data");
            NUM_USERS++;
        }
        printf("%s\n", toRecieve);
        // pthread_create(&userthreads[NUM_USERS], NULL, communicate, (void *)&users[NUM_USERS]);
    }
    
}

int main(){
    pthread_t execServerLoopThread;
    pthread_create(&execServerLoopThread, NULL, execServerLoop, (void *) NULL);
    while(1){
        if(getchar()=='q'){
            quitServerFlag=1;
            pthread_join(execServerLoopThread, (void *) NULL);
            break;
        }
    }
}