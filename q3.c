#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#define SERVERPORT 9999

int main() {
	struct sockaddr_in server_addr_block;
	bzero((char *) &server_addr_block, sizeof(server_addr_block));
	server_addr_block.sin_family = AF_INET;
	server_addr_block.sin_addr.s_addr = INADDR_ANY;
	server_addr_block.sin_port = htons(SERVERPORT);

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		printf("Invalid server descriptor\n");
		return -1;
	}

	if (bind(server_fd, (struct sockaddr *) &server_addr_block, sizeof(server_addr_block)) < 0) {
		printf("Error in binding descriptor\n");
		return -1;
	}

	listen(server_fd, 10);

	while (1) {
		struct sockaddr_in client_addr_block;
		int client_len = sizeof(client_addr_block);
		int child_fd = accept(server_fd, (struct sockaddr *) &client_addr_block, (socklen_t *) &client_len);

		if (child_fd < 0) {
			printf("Invalid server descriptor\n");
			return -1;
		}

		int pid = fork();

		if (pid < 0) {
			printf("Error in forking\n");
			return -1;
		}

		if (pid == 0) {
			close(server_fd);
			char input_buffer[1024];
			bzero(input_buffer, 1024);
			int input_length = read(child_fd, input_buffer, 1024);
			if (input_length < 0) {
				return -1;
			}
			printf("SENDING BACK\n");
			if (write(child_fd, input_buffer, input_length) < 0) {
				printf("Error returning message\n");
				return -1;
			}
			exit(0);
		}
		else
			close(child_fd);
	}
	return 0;
}
