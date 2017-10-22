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

using namespace std;
priority_queue< pair <int, string> > pq;
float rt = 0, fn[] = {0,0,0}, old_rt;
int N = 0, count[] = {0,0,0}, t=0;
time_t start, t0;
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

void check_active(){
    N = 0;
    for(int i=0;i<3;i++){
		if(fn[i] < rt)
			N++;
    }
}

void update_rt(){
    time_t current = time(0);
    float new_rt = rt + (float)(current-t0)/N;
    old_rt = rt;
    rt = new_rt;
}

void *parser(void *pp){
    while(1){
	    door.lock();
		if(!pq.size()) {
			door.unlock();
			continue;
		}
		else {
			//LOCK
			pair<int, string> packet = pq.top();
			pq.pop();
			door.unlock();
			usleep(packet.second.size()*1000);
			door.lock();
			update_rt();
			check_active();
			// send
			send_packet(packet.second);
			t0 = time(0);
			door.unlock();
			//UNLOCK
		}
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
    start = time(0);
    pthread_t sender;
    pthread_create(&sender ,NULL, parser, NULL);
    while(1){
		/* Try to receive any incoming UDP datagram. Address and port of
		   requesting client will be stored on serverStorage variable */
		recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
		// INTERRUPT TO
		// LOCK
		door.lock();
		//Insert packet into queue with FN and RN
		int temp = (int)(buffer[0] - '1');
		string str(buffer);
		t0 = time(0);
		fn[temp] = max(fn[temp], rt) + strlen(buffer);
		pq.push(make_pair(fn[temp], str));
		update_rt();
		check_active();
		door.unlock();

		//printf("rec: %s\n", buffer);
    }

    return 0;
}
