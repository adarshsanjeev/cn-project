#include<stdio.h> //For standard things
#include<stdlib.h>    //malloc
#include<string.h>    //memset

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

#define MACHINE_A_IP "10.1.34.125"
#define MACHINE_B_IP "10.2.40.185"
#define MACHINE_C_IP "127.0.0.1"

#define MACHINE_A_MAC {0xfc, 0x15, 0xb4, 0xfd, 0x21, 0x96}
#define MACHINE_B_MAC {0x00, 0xe0, 0x4c, 0x42, 0xfe, 0xd3}
#define MACHINE_C_MAC "127.0.0.1"

/* Ethernet header */
struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* Compute checksum for count bytes starting at addr, using one's complement of one's complement sum*/
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

/* void print_ip_header(unsigned char* Buffer, int Size) */
/* { */
/* 	struct sockaddr_in source,dest; */
/*     unsigned short iphdrlen; */

/*     struct iphdr *iph = (struct iphdr *)Buffer; */
/*     iphdrlen =iph->ihl*4; */

/*     memset(&source, 0, sizeof(source)); */
/*     source.sin_addr.s_addr = iph->saddr; */

/*     memset(&dest, 0, sizeof(dest)); */
/*     dest.sin_addr.s_addr = iph->daddr; */

/*     printf("\n"); */
/*     printf("IP Header\n"); */
/*     printf("   |-IP Version        : %d\n",(unsigned int)iph->version); */
/*     printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4); */
/*     printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos); */
/*     printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len)); */
/*     printf("   |-Identification    : %d\n",ntohs(iph->id)); */
/*     //printf("   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero); */
/*     //printf("   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment); */
/*     //printf("   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment); */
/*     printf("   |-TTL      : %d\n",(unsigned int)iph->ttl); */
/*     printf("   |-Protocol : %d\n",(unsigned int)iph->protocol); */
/*     printf("   |-Checksum : %d\n",ntohs(iph->check)); */
/*     printf("   |-Source IP        : %s\n",inet_ntoa(source.sin_addr)); */
/*     printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr)); */
/* } */

/* void print_icmp_packet(unsigned char* Buffer , int Size) */
/* { */
/*     unsigned short iphdrlen; */

/*     struct iphdr *iph = (struct iphdr *)Buffer; */
/*     iphdrlen = iph->ihl*4; */

/*     struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen); */

/*     printf("\n\n***********************ICMP Packet*************************\n"); */

/*     print_ip_header(Buffer , Size); */

/*     printf("\n"); */

/*     printf("ICMP Header\n"); */
/*     printf("   |-Type : %d",(unsigned int)(icmph->type)); */

/*     if((unsigned int)(icmph->type) == 11) */
/*         printf("  (TTL Expired)\n"); */
/*     else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY) */
/*         printf("  (ICMP Echo Reply)\n"); */
/*     printf("   |-Code : %d\n",(unsigned int)(icmph->code)); */
/*     printf("   |-Checksum : %d\n",ntohs(icmph->checksum)); */
/*     //printf("   |-ID       : %d\n",ntohs(icmph->id)); */
/*     //printf("   |-Sequence : %d\n",ntohs(icmph->sequence)); */
/*     printf("\n"); */

/*     printf("IP Header\n"); */
/*     PrintData(Buffer,iphdrlen); */

/*     printf("UDP Header\n"); */
/*     PrintData(Buffer + iphdrlen , sizeof icmph); */

/*     printf("Data Payload\n"); */
/*     PrintData(Buffer + iphdrlen + sizeof icmph , (Size - sizeof icmph - iph->ihl * 4)); */

/*     printf("\n###########################################################"); */
/* } */

/* void PrintData (unsigned char* data , int Size) */
/* { */
/* 	int i, j; */
/*     for(i=0 ; i < Size ; i++) */
/*     { */
/*         if( i!=0 && i%16==0)   //if one line of hex printing is complete... */
/*         { */
/*             printf("         "); */
/*             for(j=i-16 ; j<i ; j++) */
/*             { */
/*                 if(data[j]>=32 && data[j]<=128) */
/*                     printf("%c",(unsigned char)data[j]); //if its a number or alphabet */

/*                 else printf("."); //otherwise print a dot */
/*             } */
/*             printf("\n"); */
/*         } */

/*         if(i%16==0) printf("   "); */
/* 		printf(" %02X",(unsigned int)data[i]); */

/*         if( i==Size-1)  //print the last spaces */
/*         { */
/*             for(j=0;j<15-i%16;j++) printf("   "); //extra spaces */

/*             printf("         "); */

/*             for(j=i-i%16 ; j<=i ; j++) */
/*             { */
/*                 if(data[j]>=32 && data[j]<=128) printf("%c",(unsigned char)data[j]); */
/*                 else printf("."); */
/*             } */
/*             printf("\n"); */
/*         } */
/*     } */
/* } */
