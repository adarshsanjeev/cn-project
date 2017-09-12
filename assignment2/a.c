#include "print.h"
pcap_t *packet_capture_handler;

u_char SRC_MAC[6] = MACHINE_A_MAC;
u_char DST_MAC[6] = MACHINE_B_MAC;

void process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct iphdr *ip = (struct iphdr*)(packet + SIZE_ETHERNET);
    struct sniff_ethernet *ethernet = (struct sniff_ethernet*)(packet);

    if(ip->protocol == IPPROTO_ICMP){
	printf("Sending packet from A(%s)\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
	if(inet_aton( MACHINE_B_IP, (struct in_addr *)&ip->daddr) != 1){
	    printf("IP conversion fails.\n");
	    exit(-1);
	}
	for(i=0; i<6; i++){
	    ethernet->ether_shost[i] = SRC_MAC[i];
	    ethernet->ether_dhost[i] = DST_MAC[i];
	}

	size_t packet_size = ntohs(ip->tot_len) - 28;
	ip->check = 0;
	ip->check = compute_checksum((unsigned short*)ip, 4 * ip->ihl);
	fflush(stdout);
	int pcap_status = pcap_sendpacket(packet_capture_handler, packet, 50+packet_size);
	if (pcap_status != 0)
	    printf("%d Sendpacket failed\n", pcap_status);
    }
}

int main(int argc, char **argv){
    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    char* filter_exp = "icmp";
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    int nop = 10000000;
    if (argc == 2)
	dev = argv[1];
    else
	exit(-1);
    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
	net = 0;
	mask = 0;
    }
    printf("Listening for packets on device: %s\n", dev);
    packet_capture_handler = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    pcap_setdirection(packet_capture_handler, PCAP_D_OUT);
    pcap_compile(packet_capture_handler, &fp, filter_exp, 0, net);
    pcap_setfilter(packet_capture_handler, &fp); 
    pcap_loop(packet_capture_handler, nop, process, NULL);
    pcap_freecode(&fp);
    pcap_close(packet_capture_handler);
    return 0;
}
