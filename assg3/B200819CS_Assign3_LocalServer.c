#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#define myPort 9090
#define size 2000
#define capacity 100
#define not_found "Not found"

typedef struct dns_data
{
    char hostname[size];
    char ip_address[size];
} dns;

int main()
{
    int socket_desc, socket_desc_client;
    struct sockaddr_in server, root, client;
    char message[size], *root_reply;
    int slen = sizeof(client);
    int n = 0;
    root_reply = malloc(size * sizeof(char));

    dns cache[capacity];

    // To connect to clients as a server
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(myPort);
    // as a server

    // for connecting to root
    socket_desc_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc_client == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    root.sin_family = AF_INET;
    root.sin_addr.s_addr = inet_addr("127.0.0.2");
    root.sin_port = htons(myPort);
    // as a client...

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Error: Failed to bind\n");
        return 1;
    }

    int yes = 1;
    // char yes='1'; // Solaris people use this

    // lose the pesky "Address already in use" error message
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    printf("Local server initiated\nWaiting for data...\n");

    while (1)
    {

        int recv_len = recvfrom(socket_desc, message, size, 0, (struct sockaddr *)&client, &slen);
        if (recv_len == -1)
        {
            perror("Error: Failed to receive data\n");
            return 1;
        }

        // Print the received message
        printf("Received packet from %s:%d\nData from client: %s\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message);

        // search in cache
        int flag = 0, len = size;
        for (int i = 0; i < n; i++)
        {
            if (!strcmp(cache[i].hostname, message))
            {
                flag = 1;
                strcpy(root_reply, cache[i].ip_address);
                len = strlen(cache[i].ip_address);
                break;
            }
        }

        if (!flag)
        {
            // connect to root server and get the answer
            if (sendto(socket_desc_client, message, strlen(message), 0, (struct sockaddr *)&root, sizeof(root)) < 0)
            {
                perror("Error: Failed to send message to server\n");
                return 1;
            }
            printf("Sent to root server: %s\n", message);

            int slen = sizeof(root);
            len = recvfrom(socket_desc_client, root_reply, size, 0, (struct sockaddr *)&root, &slen);
            if (recv_len == -1)
            {
                perror("Error: Failed to receive message from server\n");
                return 1;
            }

            // if found, then only store in cache
            if (strcmp(root_reply, not_found))
            {
                // store in cache
                strcpy(cache[n].hostname, message);
                strcpy(cache[n].ip_address, root_reply);
                n++;
            }
        }

        // Send the ip address back to the client
        if (sendto(socket_desc, root_reply, len, 0, (struct sockaddr *)&client, slen) == -1)
        {
            perror("Error: Failed to send message\n");
            return 1;
        }

        printf("Sent back to client: %s\n\n", root_reply);
        // free(root_reply);
        memset(message, '\0', size);
        memset(root_reply, '\0', size);

        // printf("Closing the connection...\n");

        // Close the socket
        // close(socket_desc);
    }

    return 0;
}
