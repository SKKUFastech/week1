#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "Command_Buffer.h"

#define BUFFER_SIZE 258 // 최대 데이터 크기

extern char sync_num; // 프레임 동기화 번호를 외부에서 선언하여 사용합니다.
extern int client_socket; // 클라이언트 소켓을 외부에서 선언하여 사용합니다.
extern struct sockaddr_in server_addr; // 서버 주소 정보를 외부에서 선언하여 사용합니다.

int length; // framedata에서 length 이후의 data의 총 길이를 저장하는 변수
char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
char buffer_rcv[BUFFER_SIZE];  // 받은 데이터를 저장할 버퍼
ssize_t received_bytes = 0; // 수신한 데이터의 바이트 수를 저장하는 변수

int UDP_Client_Send(int FrameType)
{
    int flag, c;
    buffer_send[0] = 0xAA;
    buffer_send[2] = sync_num;
    buffer_send[3] = 0x00;

    // system("clear");
    while (1)
    {
        Command_Buffer(FrameType); // Command_Buffer 함수를 호출하여 프레임 데이터를 구성합니다.

        // 클라이언트 소켓을 통해 서버에 데이터를 전송합니다.
        int send_result = sendto(client_socket, buffer_send, length + 2, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        if (send_result < 0)
        {
            perror("sendto 실패");
        }

        // 서버로부터 데이터를 수신합니다.
        received_bytes = recvfrom(client_socket, buffer_rcv, sizeof(buffer_rcv), 0, NULL, NULL);
        if (received_bytes < 0)
        {
            perror("recvfrom 실패");
            continue;
        }

        break; // 루프 탈출
    }

    // close(client_socket); // 주석 처리된 부분은 소켓을 연결한 후에 연결을 종료하는 코드
    return 0;
}