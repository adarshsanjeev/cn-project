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

    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /* define/compute ip header offset */
    ip = (struct iphdr*)(packet + SIZE_ETHERNET);

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
    printf("         To: %s\n", inet_ntoa(*(struct in_addr*)&ip->daddr));

    /* determine protocol */	
    if (ip->protocol == IPPROTO_ICMP && strcmp(inet_ntoa(*(struct in_addr*)&ip->saddr), MACHINE_B_IP) == 0) {
		count++;
		printf("%d\n", count);
	}
}

int main(int argc, char **argv)
{
    char *dev = NULL;			/* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */

    char filter_exp[] = "icmp";		/* filter expression [3] */
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

    pcap_setdirection(handle, PCAP_D_IN);

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

