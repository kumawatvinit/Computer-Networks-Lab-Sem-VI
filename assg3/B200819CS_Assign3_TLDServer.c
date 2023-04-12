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
}dns;

int main()
{
    int socket_desc, socket_desc_client;
    struct sockaddr_in server, auth, client;
    char root_msg[size], *auth_reply;
    auth_reply = malloc(size*sizeof(char));
       
    int slen = sizeof(client);
    int n=0;
    dns cache[capacity];

    // To connect to tld as a server
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.3");
    server.sin_port = htons(myPort);
    // as a server

    // for connecting to auth 
    socket_desc_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc_client == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }
    
    auth.sin_family = AF_INET;
    auth.sin_addr.s_addr = inet_addr("127.0.0.4");
    auth.sin_port = htons(myPort);
    // as a client...

    
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

    printf("TLD server initiated\nWaiting for data...\n");

    while(1)
    {

        int recv_len = recvfrom(socket_desc, root_msg, size, 0, (struct sockaddr *) &client, &slen);
        if (recv_len == -1)
        {
            perror("Error: Failed to receive data\n");
            return 1;
        }

        // Print the received root_msg
        printf("Received packet from %s:%d\nData from root server: %s\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), root_msg);

        // search in cache
        int flag = 0, len=size;
        for(int i=0; i<n; i++)
        {
            if(!strcmp(cache[i].hostname,root_msg)) {
                flag = 1;
                strcpy(auth_reply, cache[i].ip_address);
                len = strlen(cache[i].ip_address);
                break;
            }
        }

        if(!flag)
        {
            // connect to auth server and get the answer
            if(sendto(socket_desc_client , root_msg , strlen(root_msg) , 0 , (struct sockaddr *) &auth, sizeof(auth)) < 0)
            {
                perror("Error: Failed to send root_msg to server\n");
                return 1;
            }
            printf("Sent to auth server: %s\n", root_msg);


            int slen = sizeof(auth);
            // len = recvfrom(socket_desc, auth_reply, size, 0, (struct sockaddr *) &auth, &slen);
            len = recvfrom(socket_desc_client, auth_reply, size, 0, NULL, NULL);
            if (recv_len == -1)
            {
                perror("Error: Failed to receive root_msg from server\n");
                return 1;
            }
            
            // if found, then only store in cache
            if(strcmp(auth_reply,not_found))
            {
            // store in cache
                strcpy(cache[n].hostname,root_msg);
                strcpy(cache[n].ip_address,auth_reply);
                n++;
            }
        }

        // Send the ip address back to the client
        if (sendto(socket_desc, auth_reply, len, 0, (struct sockaddr*) &client, slen) == -1)
        {
            perror("Error: Failed to send root_msg\n");
            return 1;
        }    


        printf("Sent back to root server: %s\n\n", auth_reply);
        // free(auth_reply);
        memset(root_msg, '\0', size);
        memset(auth_reply, '\0', size);

        // printf("Closing the connection...\n");

        // Close the socket
        // close(socket_desc);
    }
    
    return 0;
}
