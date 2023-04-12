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

typedef struct dns_data {
    char hostname[size];
    char ip_address[size];
}storage;

int main()
{
    int socket_desc;
    struct sockaddr_in server, client;
    char tld_query[size];
    int slen = sizeof(client);
    char * resolved;
    resolved = malloc(size*sizeof(char));
    
    int n = 0;
    storage data[capacity];

    // storing dummy data
    strcpy(data[0].hostname, "google.com");
    strcpy(data[0].ip_address, "216.58.194.174");
    
    strcpy(data[1].hostname, "github.com");
    strcpy(data[1].ip_address, "172.64.35.176");
    
    strcpy(data[2].hostname, "twitter.com");
    strcpy(data[2].ip_address, "151.101.1.140");
    
    strcpy(data[3].hostname, "stackoverflow.com");
    strcpy(data[3].ip_address, "185.199.111.153");
    
    strcpy(data[4].hostname, "amazon.com");
    strcpy(data[4].ip_address, "13.249.40.41");
    n=5;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.4");
    server.sin_port = htons(myPort);

    // Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error: Failed to bind\n");
        return 1;
    }
    
    int yes=1;
//char yes='1'; // Solaris people use this

// lose the pesky "Address already in use" error message
if (setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
} 

    printf("AUTH server initiated\nWaiting for data...\n");

    // Keep listening for data
    while(1)
    {
        

        // Try to receive some data
        int recv_len = recvfrom(socket_desc, tld_query, size, 0, (struct sockaddr *) &client, &slen);
        if (recv_len == -1)
        {
            perror("Error: Failed to receive data\n");
            return 1;
        }

        // Print the received tld_query
        printf("Received packet from %s:%d\nData from tld: %s\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), tld_query);

        int flag = 0, len=size;
        for(int i=0; i<n; i++)
        {
            if(!strcmp(data[i].hostname,tld_query)) {
                flag = 1;
                strcpy(resolved, data[i].ip_address);
                len = strlen(data[i].ip_address);
                break;
            }
        }
        if(!flag)
        {
            strcpy(resolved, not_found);
            len = strlen(resolved);
        }
        // Send the resolved back to the tld
        if (sendto(socket_desc, resolved, len, 0, (struct sockaddr*) &client, slen) == -1)
        {
            perror("Error: Failed to send resolved\n");
            return 1;
        }


        printf("Sent back to tld: %s\n\n", resolved);
        // free(resolved);
        memset(tld_query, '\0', size);
        memset(resolved, '\0', size);

        // printf("Closing the connection...\n");

        // Close the socket
        // close(socket_desc);
    }

    return 0;
}
