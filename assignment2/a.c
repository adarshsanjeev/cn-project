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
#include "print.h"

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/*
 * dissect/print packet
 */

pcap_t *handle;				/* packet capture handle */
/* Compute checksum for count bytes starting at addr, using one's complement of one's complement sum*/

u_char SRC_MAC[6] = MACHINE_A_MAC;
u_char DST_MAC[6] = MACHINE_B_MAC;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

    static int count = 1;                   /* packet counter */
    /* declare pointers to packet headers */
    struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    struct iphdr *ip;              /* The IP header */
    const char *payload;                    /* Packet payload */

    int size_ip;
    int size_payload;

    printf("\nPacket number %d:\n", count);
    count++;

    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /* define/compute ip header offset */
    ip = (struct iphdr*)(packet + SIZE_ETHERNET);

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
    printf("         To: %s\n", inet_ntoa(*(struct in_addr*)&ip->daddr));

    /* determine protocol */	
    if (ip->protocol == IPPROTO_ICMP) {
		printf("   Protocol: ICMP\n");
		printf("Sending packet\n");

		if ( inet_aton( MACHINE_B_IP, (struct in_addr *)&ip->daddr) != 1)
		{
			printf("IP conversion failes.\n");
			exit(-1);
		}

		for (int i=0; i<6; i++) {
			ethernet->ether_shost[i] = SRC_MAC[i];
			ethernet->ether_dhost[i] = DST_MAC[i];
		}

		printf("         New dest ip : %s\n", inet_ntoa(*(struct in_addr*)&ip->daddr));
		printf("         New source ip : %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));

		//size_t packet_size = sizeof(packet) + ETHER_ADDR_LEN;
		size_t packet_size = ntohs(ip->tot_len) - 28;
		char *packet_data = (u_char *)(packet + SIZE_ETHERNET + size_ip);
		printf("header len = %d packet size = %lu\n", header->len, packet_size);

		//printf("size of = %d\n", sizeof(packet[0]));
		//fflush(stdout);
		//printf("packet data = %s\n", packet_data);
		for (int i=0; i<packet_size; i++)
		{
			if(isprint(packet[i]))                /* Check if the packet data is printable */
				printf("%c ",packet[i]);          /* Print it */
			else
				printf(" . ");          /* If not print a . */
			if((i%16==0 && i!=0) || i==header->len-1)
				printf("\n");
		}

		printf("old chwcksmu = %d\n", ip->check);
		ip->check = 0;
		ip->check = compute_checksum((unsigned short*)ip, 4 * ip->ihl);
		printf("new chwcksmu = %d\n", ip->check);
		fflush(stdout);

		int pcap_status = pcap_sendpacket(handle, packet, 50+packet_size);
		if (pcap_status != 0)
			printf("%d Sendpacket failed\n", pcap_status);

		return;
    }

    return;
}

int main(int argc, char **argv)
{
    char *dev = NULL;			/* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */

    char* filter_exp = "icmp";		/* filter expression [3] */
    struct bpf_program fp;			/* compiled filter program (expression) */
    bpf_u_int32 mask;			/* subnet mask */
    bpf_u_int32 net;			/* ip */
    int num_packets = 10000000;			/* number of packets to capture */

    /* check for capture device name on command-line */
    if (argc == 2) {
        dev = argv[1];
    }
    else
		exit(EXIT_FAILURE);

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
                dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    printf("Device: %s\n", dev);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    pcap_setdirection(handle, PCAP_D_OUT);

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* now we can set our callback function */
    pcap_loop(handle, num_packets, got_packet, NULL);

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);

    printf("\nCapture complete.\n");

    return 0;
}

