#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for messages...\n");

    while (1) {
        // Receive message from client
        int len = recvfrom(server_socket, (char *)buffer, BUFFER_SIZE, 0,
                           (struct sockaddr *)&client_addr, &client_addr_size);
        buffer[len] = '\0';
        printf("Client: %s\n", buffer);

        // Reply to client
        printf("Server (You): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(server_socket, (const char *)buffer, strlen(buffer), 0,
               (const struct sockaddr *)&client_addr, client_addr_size);
    }

    close(server_socket);
    return 0;
}
