#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "169.254.14.229" // Replace with the server's IP address
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];

    while (1) {
        // Send message to server
        printf("Client (You): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(client_socket, (const char *)buffer, strlen(buffer), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive message from server
        int len = recvfrom(client_socket, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);
        buffer[len] = '\0';
        printf("Server: %s\n", buffer);
    }

    close(client_socket);
    return 0;
}
