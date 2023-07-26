#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "Command_Buffer.h"

#define BUFFER_SIZE 258 // 최대 데이터 크기

extern char sync_num;
extern int client_socket;
extern struct sockaddr_in server_addr;

char buffer[BUFFER_SIZE]; // 데이터를 저장할 버퍼
ssize_t received_bytes = 0;

int UDP_Client_Send(int FrameType)
{
    int flag, c;
    buffer[0] = 0xAA;
    buffer[2] = sync_num;
    buffer[3] = 0x00;

    // system("clear"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
    while (1)
    {
        Command_Buffer(FrameType);
        printf("RaspberryPi: ");
        for (ssize_t i = 0; i < buffer[1]+4; i++)
        {
            printf("%02X ", (unsigned char)buffer[i]);
        }

        int send_result = sendto(client_socket, buffer, 6, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        if (send_result < 0)
        {
            perror("sendto failed");
        }

        // Receive data from server
        received_bytes = recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL);
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
        break; // 루프탈출
    }

    // close(client_socket);
    return 0;
}

// do
// {
//     flag = 0;
//     printf("Client (You): ");
//     if (!fgets(buffer, sizeof(buffer), stdin))
//     {
//         printf("입력 오류가 발생하였습니다.\n");
//         flag = 1;
//         continue;
//     }
//     buffer[strcspn(buffer, "\n")] = '\0'; // 줄바꿈 문자 제거
//                                           // int send_result = sendto(client_socket, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

//     int send_result = sendto(client_socket, buffer, 6, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
//     if (send_result < 0)
//     {
//         perror("sendto failed");
//         flag = 1;
//     }
// } while (flag);