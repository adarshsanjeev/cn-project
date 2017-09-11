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

pcap_t *handle;				/* packet capture handle */
int icmp_count = 0;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

    struct iphdr *ip = (struct iphdr*)(packet + SIZE_ETHERNET);

    if (ip->protocol == IPPROTO_ICMP && strcmp(inet_ntoa(*(struct in_addr*)&ip->saddr), MACHINE_B_IP) == 0) {
		icmp_count++;
		printf("Received a packet from: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
		printf("Count: %d\n", icmp_count);
	}
}

int main(int argc, char **argv)
{
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    char filter_exp[] = "icmp";
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    int num_packets = 10000000;

    if (argc == 2)
        dev = argv[1];
    else
		exit(EXIT_FAILURE);

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
                dev, errbuf);
        net = 0;
        mask = 0;
    }

    printf("Listening for packets on device: %s\n", dev);

    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    pcap_setdirection(handle, PCAP_D_IN);

    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    pcap_loop(handle, num_packets, got_packet, NULL);

    pcap_freecode(&fp);
    pcap_close(handle);
    return 0;
}

