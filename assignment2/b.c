#include "print.h"

u_char A_MAC[6] = MACHINE_A_MAC;
u_char B_MAC[6] = MACHINE_B_MAC;
u_char C_MAC[6] = MACHINE_C_MAC;

pcap_t *packet_capture_handler;

void process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    int i;
    struct sniff_ethernet *ethernet = (struct sniff_ethernet*)(packet);
    struct iphdr *ip = (struct iphdr*)(packet + SIZE_ETHERNET);

    if(ip->protocol == IPPROTO_ICMP) {

		printf("Received packet from: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));

		if(!strcmp(inet_ntoa(*(struct in_addr*)&ip->saddr), MACHINE_A_IP)) {
			if(inet_aton( MACHINE_C_IP, (struct in_addr *)&ip->daddr) != 1) {
				printf("IP conversion fails.\n");
				exit(-1);
			}
			for(i=0; i<6; i++) {
				ethernet->ether_dhost[i] = C_MAC[i];
			}
		}
		else if(!strcmp(inet_ntoa(*(struct in_addr*)&ip->saddr), MACHINE_C_IP)) {
			if(inet_aton( MACHINE_A_IP, (struct in_addr *)&ip->daddr) != 1) {
				printf("IP conversion failes.\n");
				exit(-1);
			}
			for(i=0; i<6; i++)
				ethernet->ether_dhost[i] = A_MAC[i];
		}
		if(inet_aton( MACHINE_B_IP, (struct in_addr *)&ip->saddr) != 1) {
			printf("IP conversion failes.\n");
			exit(-1);
		}

		for(i=0; i<6; i++)
			ethernet->ether_shost[i] = B_MAC[i];

		ip->check = 0;
		ip->check = compute_checksum((unsigned short*)ip, 4 * ip->ihl);
		fflush(stdout);

		size_t packet_size = ntohs(ip->tot_len) - 28;
		if (pcap_sendpacket(packet_capture_handler, packet, 50+packet_size) != 0)
			printf("Sending packet error\n");
    }
}

int main(int argc, char **argv) {
    char *device = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (argc == 2)
		device = argv[1];
    else
		exit(-1);

    struct bpf_program fp;
    bpf_u_int32 mask, net;

    if(pcap_lookupnet(device, &net, &mask, errbuf) == -1) {net = 0;mask = 0;}

    printf("Capturing on device: %s\n", device);

    packet_capture_handler = pcap_open_live(device, SNAP_LEN, 1, 1000, errbuf);
    pcap_setdirection(packet_capture_handler, PCAP_D_IN);
    pcap_compile(packet_capture_handler, &fp, "icmp", 0, net);
    pcap_setfilter(packet_capture_handler, &fp);

    pcap_loop(packet_capture_handler, 10000000, process, NULL);

    pcap_freecode(&fp);
    pcap_close(packet_capture_handler);

    return 0;
}

