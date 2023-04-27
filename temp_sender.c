#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PACKET_TYPE_1 1
#define PACKET_TYPE_2 2
// #define PACKET_SIZE 1032
#define PAYLOAD_SIZE 1024

typedef struct packet
{
    int type;
    int seq_num;
    int payload[PAYLOAD_SIZE];
    int checksum;
} packet_t;


int calculate_checksum(packet_t *packet)
{
    int checksum = 0;
    int *p = (int *)packet;
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        checksum ^= *p++;
    }
    return checksum;
}

// function to send a packet to the specified address
void send_packet(int sockfd, struct sockaddr_in *addr, uint8_t type, uint32_t seq_num, uint8_t *payload)
{
    // create packet
    packet_t packet;
    packet.type = type;
    packet.seq_num = seq_num;
    memcpy(packet.payload, payload, PAYLOAD_SIZE);
    packet.checksum = calculate_checksum(&packet);

    // send packet
    sendto(sockfd, &packet, PACKET_SIZE, 0, (struct sockaddr *)addr, sizeof(*addr));
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // set destination address
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    // bind socket
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // continuously send packets every 100ms and 150ms
    uint32_t seq_num = 0;
    uint8_t payload[PAYLOAD_SIZE] = {0};
    while (1)
    {
        send_packet(sockfd, &servaddr, PACKET_TYPE_1, seq_num, payload);
        seq_num++;
        usleep(100000); // sleep for 100ms
        send_packet(sockfd, &servaddr, PACKET_TYPE_2, seq_num, payload);
        seq_num++;
        usleep(150000); // sleep for 150ms
    }

    // cleanup and exit
    close(sockfd);
    return 0;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PACKET_SIZE 1032
#define DATA_SIZE 1024
#define ACK_SIZE 5

#define SLEEP_INTERVAL_1 100000 // 100ms
#define SLEEP_INTERVAL_2 150000 // 150ms

struct packet {
    unsigned char type;
    unsigned int seq_no;
    char data[DATA_SIZE];
    unsigned char checksum;
};

int calc_checksum(struct packet pkt) {
    int sum = 0;
    sum += pkt.type + pkt.seq_no;
    for(int i = 0; i < DATA_SIZE; i++) {
        sum += pkt.data[i];
    }
    return sum % 256;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("Error opening socket");
        return 1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(1234);

    struct packet pkt;
    int seq_no = 0;

    while(1) {
        // Prepare the packet
        pkt.type = 1;
        pkt.seq_no = seq_no;
        // Fill data with random characters
        for(int i = 0; i < DATA_SIZE; i++) {
            pkt.data[i] = rand() % 256;
        }
        pkt.checksum = calc_checksum(pkt);

        // Send the packet
        sendto(sockfd, &pkt, PACKET_SIZE, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));

        // Wait for the acknowledgment
        struct packet ack_pkt;
        int len = sizeof(servaddr);
        if(recvfrom(sockfd, &ack_pkt, ACK_SIZE, 0, (struct sockaddr*)&servaddr, &len) == ACK_SIZE) {
            if(ack_pkt.type == 2 && ack_pkt.seq_no == seq_no) {
                // Correct acknowledgment received
                seq_no = (seq_no + 1) % 2;
            }
        }
        
        // Sleep for the appropriate interval
        usleep(seq_no == 0 ? SLEEP_INTERVAL_1 : SLEEP_INTERVAL_2);
    }

    // Close the socket
    close(sockfd);
    return 0;
}


*/