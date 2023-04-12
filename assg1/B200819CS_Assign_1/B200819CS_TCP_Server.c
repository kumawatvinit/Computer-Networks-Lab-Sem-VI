// server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define myPort 8989
#define backlog 100

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
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    char *message, client_message[2000];
    char *reverse_message;
	int opt = 1;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket\n");
        return 1;
    }
	
	// Attaching socket to port myPort
	// to avoid "failed to bind: Address already in use" error
	if(setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt\n");
		return 1;
	}

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;    // host byte order
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(myPort);    // short, network byte order
    memset(&(server.sin_zero), '\0', 8); //rest struct zero

    // Bind
    // Used to associate a socket with a port on the local machine
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error: Failed to bind\n");
        return 1;
    }
    else
    printf("bind done\n");

    // Listen
    // backlog is the number of connections allowed 
    if(listen(socket_desc , backlog) == -1)
    {
        perror("Error: Failed to listen\n");
        return 1;
    }

    // Accept and incoming connection
    printf("Waiting for incoming connections...\n\n");
    c = sizeof(struct sockaddr_in);

	int flag = 1;
	
    while(flag)
    {
    	new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    	
        printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        //Receive a message from client
        int read_size = recv(new_socket, client_message, 2000, 0);
        // recv() returns the number of bytes actually read into the buffer
        if(read_size > 0)
        {
            printf("Received: %s\n", client_message);
        }
        else {
            // -1, error
            // 0, the remote side has closed connection on you
        	perror("Error: Failed to receive message from client\n");
            continue;
        }

        // Send the reverse string
        reverse_message = reverse_string(client_message);
        // send() returns the number of bytes actually sent
        if(send(new_socket, reverse_message, strlen(reverse_message), 0) < 0)
        {
            perror("Error: Failed to send reverse message to client\n");
            free(reverse_message);
            continue;
        }

        printf("Sent: %s\n\n", reverse_message);

        free(reverse_message);
        memset(client_message, '\0', sizeof(client_message));
        
        flag=0;
    }

    if (new_socket<0)
    {
        perror("Error: Failed to accept connection\n");
        return 1;
    }

    printf("\nClosing the connection...\n");
    close(socket_desc);
    
    return 0;
}


