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
char SERVER_SOCKET_PATH[100];
char USER_NAME[200];

void * execUserLoop(){
    int USER_SOCKET_DESCRIPTOR;
    struct sockaddr_un USER_SOCKET;
    if((USER_SOCKET_DESCRIPTOR=socket(AF_UNIX, SOCK_STREAM, 0))==-1){
        perror("Socket init");
        exit(1);
    }
    USER_SOCKET.sun_family=AF_UNIX;
    sprintf(USER_SOCKET.sun_path,"%s",SERVER_SOCKET_PATH);
    if(connect(USER_SOCKET_DESCRIPTOR, (struct sockaddr *)&USER_SOCKET, strlen(USER_SOCKET.sun_path)+sizeof(USER_SOCKET.sun_family))==-1){
        perror("Connection");
        exit(1);
    }
    if(send(USER_SOCKET_DESCRIPTOR, USER_NAME, strlen(USER_NAME), 0)==-1){
        perror("Send data");
        exit(1);
    }
}

int main(int argc, char** argv){
    sprintf(SERVER_SOCKET_PATH,"../socket/%s", argv[1]);
    sprintf(USER_NAME, "%s", argv[2]);
    pthread_t execUserLoopThread;
    pthread_create(&execUserLoopThread, NULL, execUserLoop, (void *) NULL);
    while(1){
        if(getchar()=='q'){
            quitServerFlag=1;
            pthread_join(execUserLoopThread, (void *) NULL);
            break;
        }
    }
}