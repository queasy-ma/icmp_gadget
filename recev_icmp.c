#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// 计算校验和
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

// 打印数据的十六进制转储
void hex_dump(const char *desc, const void *addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char *pc = (const unsigned char *)addr;

    if (desc != NULL)
        printf("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf("  %s\n", buff);
            printf("  %04x ", i);
        }

        printf(" %02x", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    printf("  %s\n", buff);
}

// 解析 ICMP 包
void parse_icmp(char *buffer, int len) {
    struct icmp *icmp_hdr;
    icmp_hdr = (struct icmp *)(buffer + sizeof(struct iphdr));
    struct in_addr addr;
    addr.s_addr = ((struct iphdr *)buffer)->saddr;

    if (icmp_hdr->icmp_type == ICMP_ECHO) {
        printf("Received ping from %s: ID=%d Seq=%d\n",
               inet_ntoa(addr),
               ntohs(icmp_hdr->icmp_id),
               ntohs(icmp_hdr->icmp_seq));
        hex_dump("Data", icmp_hdr->icmp_data, len - sizeof(struct iphdr) - sizeof(struct icmp));
    } else if (icmp_hdr->icmp_type == ICMP_ECHOREPLY) {
        printf("Received ping reply from %s: ID=%d Seq=%d\n",
               inet_ntoa(addr),
               ntohs(icmp_hdr->icmp_id),
               ntohs(icmp_hdr->icmp_seq));
        hex_dump("Data", icmp_hdr->icmp_data, len - sizeof(struct iphdr) - sizeof(struct icmp));
    }
}

int main() {
    int sockfd;
    char buffer[1500];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    // 创建原始套接字
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 从套接字中读取 ICMP 数据包
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
        if (len < 0) {
            perror("recvfrom failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // 解析 ICMP 包
        parse_icmp(buffer, len);
    }

    close(sockfd);
    return 0;
}
