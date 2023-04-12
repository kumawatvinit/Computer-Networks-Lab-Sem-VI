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

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *message, server_reply[size];

    
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(myPort);

    
    message = (char *)malloc(size*sizeof(char));
	
	while(strlen(message) < 1) 
    {
    	printf("Enter Host name: ");
    	if(fgets(message, 2000, stdin) == NULL)
    	{
        	perror("Error: Failed to read input\n");
        	return 1;
    	}

    	
    	message[strcspn(message, "\r\n")] = 0;
    }
	
    
    if( sendto(socket_desc , message , strlen(message) , 0 , (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("Error: Failed to send message to server\n");
        return 1;
    }
    printf("Sent to local server: %s\n", message);

    
    int slen = sizeof(server);
    int recv_len = recvfrom(socket_desc, server_reply, size, 0, (struct sockaddr *) &server, &slen);
    if (recv_len == -1)
    {
        perror("Error: Failed to receive message from server\n");
        return 1;
    }

    printf("Received from local server: %s\n", server_reply);
    printf("Closing the connection...\n");
    
    close(socket_desc);

    return 0;
}
