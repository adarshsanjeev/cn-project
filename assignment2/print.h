#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/icmp.h>
#include <linux/ip.h>

#define SNAP_LEN 1518
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN	6

#define MACHINE_A_IP "10.1.35.125"
#define MACHINE_B_IP "10.1.34.125"
#define MACHINE_C_IP "10.1.34.183"

#define MACHINE_A_MAC {0x00, 0xe0, 0x4c, 0x44, 0x37, 0x83}
#define MACHINE_B_MAC {0x00, 0xe0, 0x4c, 0x42, 0xfe, 0xd3}
#define MACHINE_C_MAC {0x68, 0xf7, 0x28, 0x2e, 0xb6, 0xce}

struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];
    u_char  ether_shost[ETHER_ADDR_LEN];
    u_short ether_type;
};

static unsigned short compute_checksum(unsigned short *addr, unsigned int c) {
	register unsigned long tot = 0;
	while(c>1){tot += * addr;addr+=1;c-=2;}
	//if any bytes left, pad the bytes and add
	if(c>0){tot += ((*addr)&htons(0xFF00));}
	//Fold total sum to 16 bits: add carrier to result
	while(tot>>16){tot = (tot & 0xffff) + (tot >> 16);}
	tot = ~tot;
	return ((unsigned short)tot);
}
