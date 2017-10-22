#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "ports.h"

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

	while(1){
		/* Try to receive any incoming UDP datagram. Address and port of
		   requesting client will be stored on serverStorage variable */
        recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
		int clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

		socklen_t client_size;
		struct sockaddr_in clientAddr;
		client_size = sizeof clientAddr;

		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(DEST_PORT);
		clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);

		sendto(clientSocket,buffer,strlen(buffer),0,(struct sockaddr *)&clientAddr, client_size);

		printf("rec: %s\n", buffer);
	}

	return 0;
}
