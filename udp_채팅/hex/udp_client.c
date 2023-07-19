 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "169.254.14.229" // Replace with the server's IP address
#define PORT 8080
#define BUFFER_SIZE sizeof(int)

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

    int number, flag,c;

    while (1) {
        // Send number to server
        do{
            flag=0;
            printf("Client (You): ");
            if(!scanf("%x", &number)){
                printf("숫자를 입력하세요.");
                flag=1;
                while(c=getchar() != '\n' && c!= EOF);
            }
        }while(flag);


        sendto(client_socket, &number, sizeof(int), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive number from server
        recvfrom(client_socket, &number, sizeof(int), 0, NULL, NULL);
        printf("Server: %x\n", number);
    }

    close(client_socket);
    return 0;
}
