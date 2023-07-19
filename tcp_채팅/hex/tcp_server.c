#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "169.254.14.229"
#define PORT 8080
#define BUFFER_SIZE sizeof(int)

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    int number;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Connection established, start chatting
    while (1) {
        // Send number to server
        printf("Client: ");
        scanf("%d", &number);
        send(client_socket, &number, sizeof(int), 0);

        // Receive number from server
        int bytes_received = recv(client_socket, &number, sizeof(int), 0);
        if (bytes_received <= 0) {
            break;
        }

        // Print server's number
        printf("Server: %d\n", number);
    }

    // Close socket
    close(client_socket);
    return 0;
}
