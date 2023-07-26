#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Command.h"
#include <arpa/inet.h>

#define PORT 3001
#define BUFFER_SIZE 258 // 최대 데이터 크기

extern char sync_num;
extern int client_socket;
extern struct sockaddr_in server_addr;

int UDP_Client_Send(int FrameType)
{
    char buffer[BUFFER_SIZE]; // 데이터를 저장할 버퍼
    int flag, c;

    while (1)
    {
        do
        {
            flag = 0;
            printf("Client (You): ");
            if (!fgets(buffer, sizeof(buffer), stdin))
            {
                printf("입력 오류가 발생하였습니다.\n");
                flag = 1;
                continue;
            }
            buffer[strcspn(buffer, "\n")] = '\0'; // 줄바꿈 문자 제거
            // int send_result = sendto(client_socket, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

            if (FrameType == 1)
            {
                buffer[0] = 0xAA;
                buffer[1] = 0x04;
                buffer[2] = sync_num;
                buffer[3] = 0x00;
                buffer[4] = 0x2A;
                buffer[5] = 0x01;
            }
            else
            { // if (FrameType == 0)
                buffer[0] = 0xAA;
                buffer[1] = 0x04;
                buffer[2] = sync_num;
                buffer[3] = 0x00;
                buffer[4] = 0x2A;
                buffer[5] = 0x00;
            }

            int send_result = sendto(client_socket, buffer, 6, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
            if (send_result < 0)
            {
                perror("sendto failed");
                flag = 1;
            }
        } while (flag);

        // Receive data from server
        ssize_t received_bytes = recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if (received_bytes < 0)
        {
            perror("recvfrom failed");
            continue;
        }

        // Print the received data in hexadecimal format
        printf("Server: ");
        for (ssize_t i = 0; i < received_bytes; i++)
        {
            printf("%02X ", (unsigned char)buffer[i]);
        }
        printf("\n");
        Command();
    }

    close(client_socket);
    return 0;
}