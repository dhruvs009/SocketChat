#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void communicate(){}

void recieve(void* socket_ID){}

void send(){}

struct sockaddr_in connectServer(int socketDescriptor, int port, char* ipAddress){
    struct sockaddr_in socketAddress;
    socketAddress.sin_family=AF_INET;
    socketAddress.sin_port=htons(port);
    socketAddress.sin_addr.s_addr=inet_addr(ipAddress);
    if(connect(socketDescriptor, (struct sockaddr *) &socketAddress, sizeof(socketAddress))<0){
        perror("User connection");
        exit(0);
    }
    printf("User connected to server at %s",ipAddress);
}

int main(){
    int USER_SOCKET=socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in USER_SOCKET_ADDR=connectServer(USER_SOCKET, 3001, "127.0.0.1");
    communicate();
}