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

int length; //framedata에서 length 이후의 data의 총길이
char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
char buffer_rcv[BUFFER_SIZE];  // 받을 데이터를 저장할 버퍼
ssize_t received_bytes = 0;

int UDP_Client_Send(int FrameType)
{
    int flag, c;
    buffer_send[0] = 0xAA;
    buffer_send[2] = sync_num;
    buffer_send[3] = 0x00;

    // system("clear"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
    while (1)
    {
        Command_Buffer(FrameType);

        int send_result = sendto(client_socket, buffer_send, length+2, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        if (send_result < 0)
        {
            perror("sendto failed");
        }

        // Receive data from server
        received_bytes = recvfrom(client_socket, buffer_rcv, sizeof(buffer_rcv), 0, NULL, NULL);
        if (received_bytes < 0)
        {
            perror("recvfrom failed");
            continue;
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
//     if (!fgets(buffer_send, sizeof(buffer_send), stdin))
//     {
//         printf("입력 오류가 발생하였습니다.\n");
//         flag = 1;
//         continue;
//     }
//     buffer_send[strcspn(buffer_send, "\n")] = '\0'; // 줄바꿈 문자 제거
//                                           // int send_result = sendto(client_socket, buffer_send, strlen(buffer_send), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

//     int send_result = sendto(client_socket, buffer_send, 6, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
//     if (send_result < 0)
//     {
//         perror("sendto failed");
//         flag = 1;
//     }
// } while (flag);