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
	char buffer[1024] = "2KJHOPIHpoIUGPIUgPIHpIH[OIHS[oiSHG[OSId[oaksfn[OIKNFSKLN[N[oKN[OSDKGNLDSFKGJDLGKFDJJJJJJJJJJJJJllllllllllllllllllllllllllllllllksdfjsldkvnmsdlkn of epfojsdl;kjsd;lfsk;flskdfsd;flskdf    ;sdlfksd;flsdkfwpoejwepori pwoeirwpo ortihepg suigh wpoerioghj weporogih sepeghkns kgsjhg [oseig jed[zo gjirspoh gxfopi jhxfd;ptpo gjzdfht golkfdxtj hnrstklghser;tgjsretopgjhsrtpoighsretglidsghsrtoiughsdpioghsdpighrtglikudhgfkijlhsdkfha;klgfhnaeprirgnaepiv seobiwserterbvhsepeignqeorgfinergoienrgeoraeoigertjgertopijpoIJHOPEIRHTERP[IHTGPOIHpihap[IH[POIh[OIH[IJHRTOIGHJEROGIJEOGIJ[oijdfkgjdlfkgjeopr9tuwlem nfekrgjhertog9ijwertwerftgewrtt34978546857r34y9r834759837r-098r2e098098(*&*%!@#$%^&*()ykwjebwkeifnewrfnewrfhaer[tikl;welfks;dfvmsd;lvsdbvndsfbbfuckqwkhfdoweifjhweoifj['EWFGOPJAWRGLKRNAEGLRGKNFL,GNADSLKGDSLKFGSLDKGNLljpeoijgerpgojkgpeorjnperngbrophankakqkrennerrohankantewnensdfnsdlkfsdnsdfsdfkjsdlkllllllllllllllllllllllllllllllll;sdfks;gfjwerpgojwe[porigjhwpognwerpognwerpoginwerpogijwpotrghijwghoirtgjhoooH";
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

		usleep(3000000);
	}

	return 0;
}
