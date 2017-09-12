#include "print.h"
pcap_t *packet_capture_handler;		
int icmp_count = 0;
void process(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct iphdr *ip = (struct iphdr*)(packet + SIZE_ETHERNET);
    if(ip->protocol==IPPROTO_ICMP && strcmp(inet_ntoa(*(struct in_addr*)&ip->saddr),MACHINE_B_IP)==0){
	icmp_count++;
	printf("Received a packet from: %s\n", inet_ntoa(*(struct in_addr*)&ip->saddr));
	printf("Count: %d\n", icmp_count);
    }
}

int main(int argc, char **argv){
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filter_exp[] = "icmp";
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    int nop = 10000000;

    if (argc == 2)
	dev = argv[1];
    else
	exit(-1);

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
	net = 0;
	mask = 0;
    }
    printf("Listening for packets on device: %s\n", dev);
    packet_capture_handler = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    pcap_setdirection(packet_capture_handler, PCAP_D_IN);
    pcap_compile(packet_capture_handler, &fp, filter_exp, 0, net);
    pcap_setfilter(packet_capture_handler, &fp);
    pcap_loop(packet_capture_handler, nop, process, NULL);
    pcap_freecode(&fp);
    pcap_close(packet_capture_handler);
    return 0;
}

