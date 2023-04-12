#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 256
#define MAX_NAME_LENGTH 20
#define myPort 7777

struct client {
    int socket_fd;
    char name[MAX_NAME_LENGTH];
};

void broadcast_message(int sender_socket, struct client clients[MAX_CLIENTS], char message[MAX_MESSAGE_LENGTH]) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket_fd != -1 && clients[i].socket_fd != sender_socket) {
            send(clients[i].socket_fd, message, strlen(message), 0);
        }
    }
}


int main() {
    int listen_socket, client_socket, max_socket, i, activity, valread;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);
    char message[MAX_MESSAGE_LENGTH];
    struct client clients[MAX_CLIENTS];
    fd_set read_fds;
    
    // Initialize the clients array
    for (i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket_fd = -1;
    }
    
    // Create the listening socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        perror("Error creating the listening socket");
        exit(1);
    }
    
    // Set the server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(myPort);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    // Bind the socket to the address
    if (bind(listen_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        perror("Error binding the socket to the address");
        exit(1);
    }
    
    // Start listening for incoming connections
    if (listen(listen_socket, MAX_CLIENTS) == -1) {
        perror("Error starting to listen for incoming connections");
        exit(1);
    }
    
    while (1) {
        // Clear the file descriptor set
        FD_ZERO(&read_fds);
        // Add the listening socket to the set
        FD_SET(listen_socket, &read_fds);
        max_socket= listen_socket;

        // Add the client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++) 
        {
            if (clients[i].socket_fd != -1) 
            {
                FD_SET(clients[i].socket_fd, &read_fds);
                if (clients[i].socket_fd > max_socket) {
                    max_socket = clients[i].socket_fd;
                }
            }
        }

        // Wait for an activity on any of the sockets
        activity = select(max_socket + 1, &read_fds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Error in select");
            exit(1);
        }

        // Handle the new connection
        if (FD_ISSET(listen_socket, &read_fds)) {
            // Accept the incoming connection
            client_socket = accept(listen_socket, (struct sockaddr*) &client_address, &client_address_length);
            if (client_socket == -1) {
                perror("Error accepting the incoming connection");
                exit(1);
            }

            // Assign a name to the client
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket_fd == -1) {
                    snprintf(clients[i].name, MAX_NAME_LENGTH, "Client %d", i + 1);
                    clients[i].socket_fd = client_socket;
                    break;
                }
            }

            // Print the message in the server's terminal
            printf("New client %s joined\n", clients[i].name);

            // Broadcast the message to all clients
            snprintf(message, MAX_MESSAGE_LENGTH, "Server: %s joined the chat\n", clients[i].name);
            broadcast_message(-1, clients, message);
        }

        // Handle the incoming message
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(clients[i].socket_fd, &read_fds)) {
                // Receive the message
                valread = recv(clients[i].socket_fd, message, MAX_MESSAGE_LENGTH, 0);
                if (valread == 0) {
                    // The client has disconnected
                    printf("Client %s left\n", clients[i].name);
                    snprintf(message, MAX_MESSAGE_LENGTH, "Server: %s left the chat\n", clients[i].name);
                    broadcast_message(-1, clients, message);
                    close(clients[i].socket_fd);
                    clients[i].socket_fd = -1;
                } else {
                    // Print the message in the server's terminal
                    message[valread] = '\0';
                    printf("%s: %s\n", clients[i].name, message);

                    // Broadcast the message to all clients
                    snprintf(message, MAX_MESSAGE_LENGTH, "%s: %s\n", clients[i].name, message);
                    broadcast_message(clients[i].socket_fd, clients, message);
                }
            }
        }
    }

    // Clean up the resources
    close(listen_socket);
    return 0;
}
