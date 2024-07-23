#include "malcolm.h"


void create_arp_reply(char *src_ip, char *src_mac, char *target_ip, char *target_mac) {
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_ll socket_address;
    unsigned char buffer[42];
    struct ethhdr *eth = (struct ethhdr *)buffer;
    struct ether_arp *arp = (struct ether_arp *)(buffer + 14);

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sockfd < 0) handle_error("socket");

    memset(&ifr, 0, sizeof(struct ifreq));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0"); // Adjust the interface name if necessary
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) handle_error("ioctl");

    socket_address.sll_ifindex = ifr.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;

    // Construct Ethernet header
    sscanf(target_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &eth->h_dest[0], &eth->h_dest[1], &eth->h_dest[2],
           &eth->h_dest[3], &eth->h_dest[4], &eth->h_dest[5]);

    sscanf(src_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &eth->h_source[0], &eth->h_source[1], &eth->h_source[2],
           &eth->h_source[3], &eth->h_source[4], &eth->h_source[5]);

    eth->h_proto = htons(ETH_P_ARP);

    // Construct ARP payload
    arp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp->ea_hdr.ar_pro = htons(ETH_P_IP);
    arp->ea_hdr.ar_hln = ETH_ALEN;
    arp->ea_hdr.ar_pln = 4;
    arp->ea_hdr.ar_op = htons(ARP_REPLY);

    memcpy(arp->arp_sha, eth->h_source, ETH_ALEN);
    inet_pton(AF_INET, src_ip, arp->arp_spa);
    memcpy(arp->arp_tha, eth->h_dest, ETH_ALEN);
    inet_pton(AF_INET, target_ip, arp->arp_tpa);

    memcpy(socket_address.sll_addr, eth->h_dest, ETH_ALEN);

    if (sendto(sockfd, buffer, 42, 0, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0)
        handle_error("sendto");

    close(sockfd);
}