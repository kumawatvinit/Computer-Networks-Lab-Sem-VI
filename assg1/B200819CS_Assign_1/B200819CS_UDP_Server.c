// server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define myPort 8888

char* reverse_string(char* str)
{
    int len = strlen(str);
    char *result = (char *)malloc((len+1)*sizeof(char));

    int i;
    for(i = 0; i < len; i++)
    {
        result[i] = str[len-i-1];
    }

    result[len] = '\0';
    return result;
}

int main()
{
    int socket_desc;
    struct sockaddr_in server, client;
    char message[2000];
    int slen = sizeof(client);
    char * reverse_message;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(myPort);

    // Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error: Failed to bind\n");
        return 1;
    }

    printf("Waiting for data...\n");

    // Keep listening for data
    // while(1)
    // {
        // Try to receive some data
        int recv_len = recvfrom(socket_desc, message, 2000, 0, (struct sockaddr *) &client, &slen);
        if (recv_len == -1)
        {
            perror("Error: Failed to receive data\n");
            return 1;
        }

        // Print the received message
        printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message);

        reverse_message = reverse_string(message);
        // Send the reversed message back to the client
        if (sendto(socket_desc, reverse_message, recv_len, 0, (struct sockaddr*) &client, slen) == -1)
        {
            perror("Error: Failed to send message\n");
            return 1;
        }


        printf("Sent: %s\n\n", reverse_message);

        free(reverse_message);
        memset(message, '\0', sizeof(message));

        printf("Closing the connection...\n");

        // Close the socket
        close(socket_desc);
    // }

    return 0;
}
