// client code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define myPort 8989

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *message, server_reply[2000];

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(myPort);

    //Connect to remote server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("connect error\n");
        return 1;
    }

    printf("Connected\n");

    // Send a message to the server
    int size = 200;
    message = (char *)malloc(size*sizeof(char));

    // Get message to send from user
    
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
    
    
    
    
    if( send(socket_desc , message , strlen(message) , 0) < 0)
    {
        perror("Error: Failed to send message to server\n");
        return 1;
    }
    printf("Sent: %s\n", message);

    // Receive a reply from the server
    if( recv(socket_desc, server_reply , 2000 , 0) < 0)
    {
        perror("Error: Failed to receive message from server\n");
        return 1;
    }
    printf("Reply received: %s\n", server_reply);

    printf("\nClosing the connection...\n");
    close(socket_desc);
    
    return 0;
}
