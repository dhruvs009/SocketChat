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

void * serverMessage(void * pid){
    int pidParent=*((int *) pid);
    char toSend[1024];
    while(quitServerFlag==0){
        fgets(toSend, 1024, stdin);
        if(strcmp(toSend, "quit\n")==0){
            quitServerFlag=1;
            int QUIT_SOCKET_DESCRIPTOR;
            struct sockaddr_un QUIT_SOCKET;
            if((QUIT_SOCKET_DESCRIPTOR=socket(AF_UNIX, SOCK_STREAM, 0))==-1){
                perror("Socket init");
                exit(1);
            }
            QUIT_SOCKET.sun_family=AF_UNIX;
            sprintf(QUIT_SOCKET.sun_path,"../socket/socket_%d",pidParent);
            if(connect(QUIT_SOCKET_DESCRIPTOR, (struct sockaddr *)&QUIT_SOCKET, strlen(QUIT_SOCKET.sun_path)+sizeof(QUIT_SOCKET.sun_family))==-1){
                perror("Connection");
            }
            if(send(QUIT_SOCKET_DESCRIPTOR, "Server Quit.", 14, 0)==-1){
                perror("Quit Handshake");
            }
            for(int i=0; i<NUM_USERS; i++){
                if(strcmp(users[i].username,"")!=0){
                    if(send(users[i].socket_ID, "@server quit.", 15, 0)==-1){
                        perror("Quit user handshake");
                    }
                }
            }
            close(QUIT_SOCKET_DESCRIPTOR);
        }
        else if(strcmp(toSend, "List Users\n")==0){
            int count=1;
            for(int i=0; i<NUM_USERS; i++){
                if(strcmp(users[i].username,"")!=0){
                    printf("%d. %s\n", count++, users[i].username);
                }
            }
        }
    }
    return NULL;
}

void * recieverLoop(void * userDetails){
    struct USER* senderUser= (struct USER *)userDetails;
    char toRecieve[1024];
    char toSend[1024];
    int RECIEVE_DESCRIPTOR;
    char * split;
    char splitted[1024][1024];
    while(quitServerFlag==0){
        RECIEVE_DESCRIPTOR=recv(senderUser->socket_ID, toRecieve, 1024, 0);
        toRecieve[RECIEVE_DESCRIPTOR]='\0';
        if(RECIEVE_DESCRIPTOR>0){
            char toProcess[1024];
            strcpy(toProcess, toRecieve);
            split=strtok(toRecieve," \n");
            int i=0;
            while(split!=NULL){
                strcpy(splitted[i], split);
                i++;
                split=strtok(NULL, " \n");
            }
            char toSendOut[1500];
            sprintf(toSendOut,"%s:", senderUser->username);
            if(i>=2){
                for(int j=1; j<i; j++){
                    strcat(toSendOut," ");
                    strcat(toSendOut, splitted[j]);
                }
            }
            if(strcmp(toProcess,"@server I quit.\n")==0){
                if(send(senderUser->socket_ID, "User exit.", 12, 0)==-1){
                    perror("User exit message");
                    strcpy(senderUser->username, "");
                    break;
                }
            }
            else if(splitted[0][0]=='@'){
                int sent=0;
                if(strcmp(splitted[0],"@everyone")==0){
                    for(int j=0; j<NUM_USERS; j++){
                        if(strcmp(users[j].username,senderUser->username)!=0 && send(users[j].socket_ID, toSendOut, strlen(toSendOut), 0)==-1){
                            perror("User send message to everyone.");
                        }
                    }
                }
                else{
                    for(int j=0; j< NUM_USERS; j++){
                        unsigned int k=strcmp(splitted[0],users[j].username); 
                        if(k==0 || k==-127){
                            if(strcmp(users[j].username,"")!=0 && send(users[j].socket_ID, toSendOut, strlen(toSendOut), 0)==-1){
                                perror("User send message.");
                            }
                            else{
                                sent=1;
                            }
                        }
                    }
                    if(sent==0){
                        if(send(senderUser->socket_ID, "@server: No such user as specified.", 37, 0)==-1){
                            perror("Server send no user message.");
                        }
                    }
                }
            }
            else{
                if(send(senderUser->socket_ID, "@server: Please specify who the message is meant for in the first word of the message preceded by @ (For example if the message is meant for username 'abc' then send the message as @abc <message>).",199, 0)==-1){
                    perror("Server send error message");
                }
            }
        }
    }
    return NULL;
}

int main(){
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
    int pid=getpid();
    pthread_t communicateThread;
    pthread_create(&communicateThread, NULL, serverMessage, (void *)&pid);
    while(quitServerFlag==0){
        users[NUM_USERS].socket_ID=accept(SERVER_SOCKET_DESCRIPTOR, (struct sockaddr *)&users[NUM_USERS].userAddress, &users[NUM_USERS].len);
        RECIEVE_DESCRIPTOR=recv(users[NUM_USERS].socket_ID, toRecieve, 200, 0);
        if(RECIEVE_DESCRIPTOR<0){
            perror("Recieve data");
        }
        else if(RECIEVE_DESCRIPTOR>0){
            if(strcmp(toRecieve,"Server Quit.")==0){
                printf("%s\n",toRecieve);
                break;
            }
            else{
                sprintf(users[NUM_USERS].username,"@%s",toRecieve);
                char towrite[400];
                NUM_USERS++;
            }
        }
        pthread_create(&userthreads[NUM_USERS], NULL, recieverLoop, (void *) &users[NUM_USERS-1]);
    }
    for(int i=0; i<NUM_USERS; i++){
        pthread_cancel(userthreads[i]);
        pthread_join(userthreads[i], (void *)NULL);
    }
    close(SERVER_SOCKET_DESCRIPTOR);
    return 0;
}