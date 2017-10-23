#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ports.h"
#include <stdlib.h>

int main(){
	int clientSocket, nBytes;
	char buffer[1024] = "3kjegtkewrghelgijealrgjlknv;kasdjvnba;fojh pfoiawejn vfqwpjHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH";
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(ROUTER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverAddr;

	while(1){
		printf("You typed: %s\n",buffer);

		nBytes = rand()%10/2;

		sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);

		usleep(1000000);
	}

	return 0;
}
