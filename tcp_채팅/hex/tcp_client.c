#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE sizeof(int)

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int number;

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    int client_addr_len = sizeof(client_addr);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Connection established, start chatting
    while (1) {
        // Receive number from client
        recv(client_socket, &number, sizeof(int), 0);

        // Print client's number
        printf("Client: %d\n", number);

        // Send number to client
        printf("Server: ");
        scanf("%d", &number);
        send(client_socket, &number, sizeof(int), 0);
    }

    // Close sockets
    close(client_socket);
    close(server_socket);
    return 0;
}
