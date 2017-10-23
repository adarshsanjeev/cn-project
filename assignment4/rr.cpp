#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <arpa/inet.h>
#include "ports.h"
#include <queue>
#include <thread>
#include <ctime>
#include <mutex>
#include <algorithm>
#include <unistd.h>
#include <iostream>
using namespace std;
queue<string> q1;
queue<string> q2;
queue<string> q3;
mutex door;
void send_packet(string buff){

    int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    socklen_t client_size;
    struct sockaddr_in clientAddr;
    client_size = sizeof clientAddr;

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(DEST_PORT);
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);
    const char *buffer = buff.c_str();
    sendto(clientSocket,buffer,strlen(buffer),0,(struct sockaddr *)&clientAddr, client_size);
}

void *parser(void *pp){
    while(1){
	sleep(1);
	door.lock();
	if(!q1.empty()){
		    send_packet(q1.front());
		    q1.pop();
		}
	if(!q2.empty()){
		    send_packet(q2.front());
		    q2.pop();
		}
	if(!q3.empty()){
		    send_packet(q3.front());
		    q3.pop();
		}
	door.unlock();
    }
}

int main(){
    int udpSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    /*Create UDP socket*/
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(ROUTER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Bind socket with address struct*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverStorage;
    pthread_t sender;
    pthread_create(&sender ,NULL, parser, NULL);
    while(1){
	recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
	int temp = (int)(buffer[0] - '1');
	string str(buffer);
	// LOCK
	door.lock();
	switch(temp){
	    case 0: q1.push(str);
		    break;
	    case 1: q2.push(str);
		    break;
	    case 2: q3.push(str);
		    break;
	}
	door.unlock();
    }
    return 0;
}
