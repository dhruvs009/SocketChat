#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct USER{
    char username[20];
    int socket_ID;
    struct sockaddr_in userAddress;
    int len;
    int status;
};

struct USER users[10];
pthread_t userthreads[10];
int quitServerFlag=0;
int NUM_USERS=0;

void * communicate(void * user){        //Recieve name from user first then set up required communication.

}

struct sockaddr_in setupServer(int socketDescriptor, unsigned long port, char* ipAddress){
    struct sockaddr_in socketAddress;
    socketAddress.sin_family=AF_INET;
    socketAddress.sin_port=htons(port);
    socketAddress.sin_addr.s_addr=inet_addr(ipAddress);
    if(bind(socketDescriptor, (struct sockaddr *) &socketAddress, sizeof(socketAddress))<0){
        perror("Socket binding");
        exit(0);
    }
    if(listen(socketDescriptor,1024)<0){
        perror("Socket listening");
        exit(0);
    }
    printf("Server on at address %s listening at port %ld\n", ipAddress, port);
    return socketAddress;
}

void * quitServer(){
    while(1){
        if(getchar()=='q'){
            quitServerFlag=1;
            return NULL;
        }
    }
}

void connectUser(int socketDescriptor){
    users[NUM_USERS].socket_ID=accept(socketDescriptor, (struct sockaddr *)&users[NUM_USERS].userAddress, &users[NUM_USERS].len);
    pthread_create(&userthreads[NUM_USERS], NULL, communicate, (void *)&users[NUM_USERS]);
    NUM_USERS++;
}

int main(){
    int SERVER_SOCKET=socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in SERVER_SOCKET_ADDR=setupServer(SERVER_SOCKET, 3001, "127.0.0.1");
    pthread_t quitServerThread;
    pthread_create(&quitServerThread, NULL, quitServer, (void *) NULL);
    while(quitServerFlag==0){
        connectUser(SERVER_SOCKET);
    }
    for(int i=0; i<NUM_USERS; i++){
        pthread_join(userthreads[i], NULL);
    }
}