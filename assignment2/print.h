#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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

static unsigned short compute_checksum(unsigned short *addr, unsigned int count) {
	register unsigned long sum = 0;
	while (count > 1) {
		sum += * addr++;
		count -= 2;
	}
	//if any bytes left, pad the bytes and add
	if(count > 0) {
		sum += ((*addr)&htons(0xFF00));
	}
	//Fold sum to 16 bits: add carrier to result
	while (sum>>16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	//one's complement
	sum = ~sum;
	return ((unsigned short)sum);
}
