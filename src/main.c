#include "malcolm.h"

void usage(char *progname) {
    fprintf(stderr, "Usage: %s <source ip> <source mac> <target ip> <target mac>\n", progname);
    exit(EXIT_FAILURE);
}

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage(argv[0]);
    }

    char *src_ip = argv[1];
    char *src_mac = argv[2];
    char *target_ip = argv[3];
    char *target_mac = argv[4];

    create_arp_reply(src_ip, src_mac, target_ip, target_mac);

    return 0;
}