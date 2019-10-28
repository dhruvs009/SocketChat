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
volatile int quitUserFlag=0;
int NUM_USERS=0;
char SERVER_SOCKET_PATH[100];
char USER_NAME[200];

void * senderLoop(void * USER_SOCKET_DESCRIPTOR){
    char toSend[1024];
    while(quitUserFlag==0){
        fgets(toSend, 1024, stdin);
        if(send(*((int *)USER_SOCKET_DESCRIPTOR), toSend, strlen(toSend), 0)==-1){
            perror("Send message");
        }
    }
    return NULL;
}

int main(int argc, char** argv){
    if(argc<3){
        printf("Please provide both socket file and Username.\n");
        exit(0);
    }
    sprintf(SERVER_SOCKET_PATH,"../socket/%s", argv[1]);
    sprintf(USER_NAME, "%s", argv[2]);
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
        perror("Send Username");
        exit(1);
    }
    pthread_t senderThread;
    pthread_create(&senderThread, NULL, senderLoop, (void * )&USER_SOCKET_DESCRIPTOR);
    while(1){
        char toRecieve[1024];
        int RECIEVE_DESCRIPTOR=recv(USER_SOCKET_DESCRIPTOR, toRecieve, 1024, 0);
        toRecieve[RECIEVE_DESCRIPTOR]='\0';
        if(strcmp(toRecieve,"User exit.")==0){
            printf("User exit.\n");
            quitUserFlag=1;
            break;
        }
        else if(strcmp(toRecieve,"@server quit.")==0){
            printf("Server was closed.\n");
            quitUserFlag=1;
            break;
        }
    }
    pthread_cancel(senderThread);
    pthread_join(senderThread, (void *)NULL);
    close(USER_SOCKET_DESCRIPTOR);
    return 0;
}