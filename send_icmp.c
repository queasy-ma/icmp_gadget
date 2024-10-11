#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// 计算ICMP包的校验和
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <destination_ip> <type> <identifier> <sequence_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *dest_ip = argv[1];
    int type = atoi(argv[2]);
    int identifier = atoi(argv[3]);
    int sequence_number = atoi(argv[4]);

    int sockfd;
    struct sockaddr_in dest_addr;
    char packet[64];
    struct icmphdr *icmp_hdr = (struct icmphdr *) packet;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);

    memset(packet, 0, sizeof(packet));
    icmp_hdr->type = type;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = htons(identifier);
    icmp_hdr->un.echo.sequence = htons(sequence_number);
    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = checksum(packet, sizeof(packet));

    if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("ICMP packet sent to %s with type %d, identifier %d, and sequence number %d\n", dest_ip, type, identifier, sequence_number);

    close(sockfd);
    return 0;
}
