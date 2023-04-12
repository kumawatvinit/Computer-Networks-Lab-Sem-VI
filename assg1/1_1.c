// with io validation and error handling

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    char client_message[2000];
    char *reverse_message;

    // Create socket
    socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        perror("Error: Failed to create socket");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error: Failed to bind");
        return 1;
    }

    // Listen
    if(listen(socket_desc , 3) == -1)
    {
        perror("Error: Failed to listen");
        return 1;
    }

    // Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
        printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Receive a message from client
        int read_size = recv(new_socket, client_message, 2000, 0);
        if(read_size < 0)
        {
            perror("Error: Failed to receive message from client");
            continue;
        }

        // Validate the input
        if(!validate_input(client_message))
        {
            char *error_message = "Error: Invalid input, please send a valid string\n";
            send(new_socket, error_message, strlen(error_message), 0);
            continue;
        }

        // Send the reverse string
        reverse_message = reverse_string(client_message);
        if(send(new_socket, reverse_message, strlen(reverse_message), 0) < 0)
        {
            perror("Error: Failed to send reverse message to client");
            free(reverse_message);
            continue;
        }
        printf("Sent: %s\n", reverse_message);

        free(reverse_message);
    }

    if (new_socket<0)
    {
        perror("Error: Failed to accept connection");
        return 1;
    }
    close(socket_desc);
    
    return 0;
}

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