#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Print_Command_List.h"

#define SERVER_IP_SIZE 14
#define BUFFER_SIZE 258

extern ssize_t received_bytes;
extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
extern char buffer_rcv[BUFFER_SIZE];  // 받을 데이터를 저장할 버퍼

// Command 함수 정의
int Command()
{
    unsigned int FrameType; // 사용자로부터 입력받은 프레임 타입을 저장할 변수
    do
    {
        // 사용자로부터 프레임 타입을 16진수 형식으로 입력 받습니다.
        printf("Please enter the Command(Frame Type) in Hex.\n");
        printf("FF: quit, FE: show list of commands\n");
        printf("Input(ex:2A): ");
        if (scanf("%x", &FrameType) == 0) // 사용자 입력이 16진수 형식이 아니면 입력 버퍼를 비웁니다.
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input format. Please try again.\n"); // 오류 메시지 출력
        }
        else // 16진수로 정상적으로 입력되었다면
        {
            break; // 반복문을 빠져나가고 입력 받은 프레임 타입 사용
        }

    } while (1); // 사용자가 유효한 프레임 타입을 입력할 때까지 반복

    return FrameType;
}