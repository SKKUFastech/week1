#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 258

extern ssize_t received_bytes; // 수신한 데이터의 바이트 수를 저장하기 위한 외부 변수
extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
extern char buffer_rcv[BUFFER_SIZE];  // 받은 데이터를 저장할 버퍼

void Monitor()
{
    // 커서를 13번째 줄의 첫 번째 열로 이동하여 Monitor 메시지를 출력합니다.
    printf("\033[13;1H");
    printf("**Monitor**\n");

    // 커서를 15번째 줄의 첫 번째 열로 이동하여 RaspberryPi 데이터를 출력합니다.
    printf("\033[15;1H");
    printf("RaspberryPi: ");
    for (ssize_t i = 0; i < buffer_send[1] + 2; i++)
    {
        // 보낼 데이터를 16진수 형식으로 출력합니다.
        printf("%02X ", (unsigned char)buffer_send[i]);
    }

    // 커서를 17번째 줄의 첫 번째 열로 이동하여 서버로부터 받은 데이터를 출력합니다.
    printf("\n\nServer: ");
    for (ssize_t i = 0; i < received_bytes; i++)
    {
        // 받은 데이터를 16진수 형식으로 출력합니다.
        printf("%02X ", (unsigned char)buffer_rcv[i]);
    }

    printf("\n");
    
    // buffer_rcv[5]가 1인 경우 "Response: OK"를 출력합니다.
    if (buffer_rcv[5] == 1)
        printf("Response: OK\n");

    // 커서를 첫 번째 줄의 첫 번째 열로 이동합니다.
    printf("\033[1;1H");
}
