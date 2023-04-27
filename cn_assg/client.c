#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<time.h>

#define one 1
#define two 2
#define payload_size 1024
#define port 5565

typedef struct data {
    int type;
    int num;
    char payload[payload_size];
    
}packet;

typedef struct {
    packet p;
    int checksum;
}finalpacket;

int calculateChecksum(packet *curr)
{
    int checksum = 0;
    char *p = (char *)curr;
    int size = sizeof(curr);


    for(int i=0; i<size; i++)
    {
        checksum^= p[i];
    }

    return checksum;
}

void printPacket(packet * p)
{
    printf("Num: %d, ", p->num);
    printf("Type: %d, ", p->type);
    printf("Payload: %s\n", p->payload);
}

void sendPacket(int sockfd, struct sockaddr_in *addr, int type, int num, char *payload)
{
    // packet banao
    packet current;
    current.type = type;
    current.num = num;

    memcpy(current.payload, payload, sizeof(payload_size));
    current.checksum = calculateChecksum(&current);

    int size = sizeof(current);
    if(sendto(sockfd, &current, size, 0, (struct sockaddr *)addr, sizeof(*addr)) < 0)
    {
        perror("Error: Failed to send message to server\n");
        return;
    }
    
    printPacket(&current);
    printf("Successfully sent\n");
}

int main()
{
    int sock_desc;
    struct sockaddr_in server;
    

    sock_desc = socket(AF_INET, SOCKDGRAM, 0);
    if(sock_desc == -1) {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    // setting destination 
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(Port);


    int t = 0;
    int totalpackets = 0;

    while(1)
    {
        usleep(1000);
        t+= 1; 

        // 100ms
        if(t%100 == 0) {
            // send type 1

            // creating random payload
            char payload[payload_size];
            for(int i=0; i<10; i++) {
                payload[i] = 'A' +  rand()%26;
            }

            sendPacket(sock_desc, server, one, totalpackets+1, payload);

            totalpackets++;
        }
        if(t%150 == 0) {
            // send type 2

            // random payload
            char payload[payload_size];
            for(int i=0; i<payload_size; i++) {
                payload[i] = rand()%256;
            }

            sendPacket(sock_desc, server, two, totalpackets+1, payload);

            totalpackets++;
        }
    }


    close(sock_desc);

    return 0;
}