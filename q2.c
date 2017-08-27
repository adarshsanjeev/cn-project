#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<limits.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
typedef int ll;
typedef struct messaging{ 
    ll size;
    char *buffer;
}data;

data readLine(){
    data MSG;
    MSG.size = 1024;
    MSG.buffer = malloc(MSG.size*sizeof(char));
    ll POS = 0, c;
    while(true){
	c = getchar();
	if(c==EOF || c=='\n'){
	    MSG.buffer[POS] = '\0';
	    return MSG;
	}
	else
	    MSG.buffer[POS] = c;
	POS+=1;
	if(POS>=MSG.size && POS < INT_MAX){
	    MSG.size += 1024;
	    MSG.buffer = realloc(MSG.buffer, MSG.size);
	}
    }
}
void sender(struct sockaddr_in sad, char *message, ll len){
    ll sockid;
    if(sockid=socket(AF_INET, SOCK_STREAM, 0)<0)
	printf("ERROR - Could not open server-socket\n");
    if(connect(sockid, (struct sockaddr *)&sad, sizeof(sad)) < 0)
	printf("ERROR - Could not connect\n");
    if(write(sockid, message, len)<0)
	printf("ERROR - Could not send message");
    return;
}
/*Client Side*/
void *client_func(void *input){
    char **arguments = (char**)(input);
    char *user = arguments[0];
    char *handle = malloc(2+strlen(user));
    strcpy(handle, user);strcat(handle, ": ");
    // Command line message
    data msg;
    // Socket Initialize
    ll sockid, n;
    struct sockaddr_in sad;
    bzero((char *) &sad, sizeof(sad));
    ll cport = atoi(arguments[3]);
    struct hostent *server = gethostbyname(arguments[1]);
    if(server==NULL)
	printf("Error - no host\n");

    //////////// Source : c tut point ////////////
    bcopy((char *)server->h_addr, (char *)&sad.sin_addr.s_addr, server->h_length);
    sad.sin_family = AF_INET;
    sad.sin_port = htons(cport);
    /////////////////////////////////////////////////

    while(true){
	printf("> ");
	msg = readLine();
	ll message_size = strlen(handle) + msg.size + 6;
	char *message = malloc(sizeof(char)*(strlen(handle)+msg.size+10));
	strcpy(message, handle);
	strcat(message, msg.buffer);
	//printf("%d",(int)strlen(message));
	if(strlen(message) == 6)
	    continue;
	sender(sad, message, message_size);
	free(msg.buffer);
    }
}

/*Server Side*/
void *server_func(void *input){
    char **arguments = (char**)input;
    char *buffer = malloc(sizeof(char) * 256);
    ll sport = atoi(arguments[2]);
    //const unsigned int
    ll sockid, csock;
    socklen_t clen;
    struct sockaddr_in sad, cad;
    //init and open
    sockid = socket(AF_INET, SOCK_STREAM, 0);
    //bind
    bzero((char *) &sad, sizeof(sad));
    sad.sin_family = AF_INET;  
    sad.sin_port = htons(sport);
    sad.sin_addr.s_addr = INADDR_ANY;  
    bind(sockid,(struct sockaddr *)&sad , sizeof(sad));
    //listen
    listen(sockid , 3);
    //accept
    clen = sizeof(cad);
    while((csock = accept(sockid,(struct sockaddr *)&cad, &clen))){
	ll bite = 0;
	bzero(buffer,256);
	bite = read(csock, buffer, 256);
	char *op = malloc(sizeof(char) * 256);
	strcpy(op, buffer);
	//display
	printf("\n%s\n",op);
    }
}

int main(ll argc, char *argv[]){
    if(argc != 4)
	printf("Provide Ip adress, sender+recv port\n");
    char *input = argv;
    pthread_t server_thread;
    pthread_t client_thread;
    pthread_create(&server_thread, NULL, server_func, (void *)input);
    pthread_create(&client_thread, NULL, client_func,(void *)input);
    pthread_join(server_thread, NULL);
    pthread_join(client_thread, NULL);
    return 0;
}
