#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 258 // 최대 데이터 크기
int FAS_GetboardInfo();
int FAS_ServoEnable();
int FAS_MoveStop();
int FAS_MoveVelocity();

extern int length; // framedata에서 length 이후의 data의 총길이
extern char sync_num;
extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
int speed;

int Command_Buffer(int FrameType)
{
    switch (FrameType)
    {
    case 0x01:
        FAS_GetboardInfo();
        break;
    case 0x2A:
        FAS_ServoEnable();
        break;
    case 0x31:
        FAS_MoveStop();
        break;
    case 0x37:
        FAS_MoveVelocity();
        break;
    }
    return 0;
}

int FAS_GetboardInfo()
{
    printf("\033[14;1H");
    printf("0x01: FAS_GetboardInfo\n");
    printf("\033[1;1H");
    buffer_send[1] = 0x03;
    buffer_send[4] = 0x01;
    length = 3;
}

int FAS_ServoEnable()
{
    printf("\033[14;1H");
    printf("0x2A: FAS_ServoEnable\n");
    printf("\033[7;1H");
    printf("-Enable\nON(1), OFF(0): ");
    scanf("%x", &buffer_send[5]);

    buffer_send[1] = 0x04;
    buffer_send[4] = 0x2A;
    length = 4;
}

int FAS_MoveStop()
{
    printf("\033[14;1H");
    printf("0x31: FAS_MoveStop\n");
    printf("\033[7;1H");

    buffer_send[1] = 0x03;
    buffer_send[4] = 0x31;
    length = 3;
}

int FAS_MoveVelocity()
{
    printf("\033[14;1H");
    printf("0x37: FAS_MoveVelocity\n");
    printf("\033[7;1H");
    printf("-Speed: ");
    scanf("%d", &speed);

    buffer_send[1] = 0x08;
    buffer_send[4] = 0x37;
    buffer_send[5] = (speed & 0xFF); // 거꾸로 들어감.
    buffer_send[6] = ((speed >> 8) & 0xFF);
    buffer_send[7] = ((speed >> 16) & 0xFF);
    buffer_send[8] = ((speed >> 24) & 0xFF);
    length = 8;

    printf("\n-Direction\n");
    printf("+Jog(1), -Jog(0): ");
    scanf("%x", &buffer_send[9]);

    // printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", buffer_send[0], buffer_send[1], buffer_send[2], buffer_send[3], buffer_send[4], buffer_send[5], buffer_send[6], buffer_send[7], buffer_send[8], buffer_send[9]);
}
