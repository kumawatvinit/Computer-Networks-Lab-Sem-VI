// client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define myPort 8888
#define size 2000

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *message, server_reply[size];

    //Create socket
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(myPort);

    // Get message to send from user
    message = (char *)malloc(size*sizeof(char));
	
	while(strlen(message) < 1) 
    {
    	printf("Enter a message to send to the server: ");
    	if(fgets(message, 2000, stdin) == NULL)
    	{
        	perror("Error: Failed to read input\n");
        	return 1;
    	}

    	// Remove newline character from message
    	message[strcspn(message, "\r\n")] = 0;
    }
	
    // Send a message to the server
    if( sendto(socket_desc , message , strlen(message) , 0 , (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("Error: Failed to send message to server\n");
        return 1;
    }
    printf("Sent: %s\n", message);

    // Receive a reply from the server
    int slen = sizeof(server);
    int recv_len = recvfrom(socket_desc, server_reply, size, 0, (struct sockaddr *) &server, &slen);
    if (recv_len == -1)
    {
        perror("Error: Failed to receive message from server\n");
        return 1;
    }

    printf("Received: %s\n", server_reply);

    printf("Closing the connection...\n");
    // Close the socket
    close(socket_desc);

    return 0;
}
