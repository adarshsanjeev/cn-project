#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SV_HOST "127.0.0.1"
#define SV_PORT  0
#define BC_HOST "127.255.255.255"
#define BC_PORT 9097

int main() {
	int server_fd, so_broadcast = 1;
	struct sockaddr_in server_addr, broadcast_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SV_HOST);
	server_addr.sin_port = SV_PORT;

	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = inet_addr(BC_HOST);
	broadcast_addr.sin_port = htons(BC_PORT);

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd == -1) {
		printf("socket creation error");
		return -1;
	}

	setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))  == -1) {
		printf("socket binding error");
		return -1;
	}

	while(1) {
		char *message= "This is a broadcast";
		if (sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) == -1)
			printf("error sending broadcast");
		sleep(3);
	}

	return 0;
}
