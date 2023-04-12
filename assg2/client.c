#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MSG_LEN 256
#define myPort 7777

int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_address;
    char message[MAX_MSG_LEN];
    
    // Create the client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create the client socket");
        exit(1);
    }
    
    // Configure the server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(myPort);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Failed to connect to the server");
        exit(1);
    }
    
    // Send and receive messages
    while (1) {
        memset(message, 0, MAX_MSG_LEN);
        if (recv(client_socket, message, MAX_MSG_LEN, 0) == -1) {
            perror("Failed to receive the message");
            break;
        }
        
        if (strlen(message) > 0) {
            printf("Received: %s\n", message);
        }
        
        memset(message, 0, MAX_MSG_LEN);
        printf("Enter a message: ");
        fgets(message, MAX_MSG_LEN, stdin);
        message[strlen(message) - 1] = '\0';
        
        if (strcmp(message, "/exit") == 0 || strcmp(message, "/quit") == 0 || strcmp(message, "/part") == 0) {
            break;
        }
        
        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Failed to send the message");
            break;
        }
    }
    
    // Close the socket
    close(client_socket);
    return 0;
}
