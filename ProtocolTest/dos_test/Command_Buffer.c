#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 258 // 최대 데이터 크기
int FAS_GetboardInfo();
int FAS_ServoEnable();
int FAS_MoveVelocity();

extern char sync_num;
extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
int temp_num;

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
    case 0x37:
        FAS_MoveVelocity();
        break;
    }
    return 0;
}

int FAS_GetboardInfo()
{
    printf("0x01: FAS_GetboardInfo\n");
    buffer_send[1] = 0x03;
    buffer_send[4] = 0x01;
}

int FAS_ServoEnable()
{
    printf("0x2A: FAS_ServoEnable\n");
    printf("-Enable\nON(1), OFF(0): ");
    scanf("%x", &buffer_send[5]);

    buffer_send[1] = 0x04;
    buffer_send[4] = 0x2A;
}

int FAS_MoveVelocity()
{
    printf("0x37: FAS_MoveVelocity\n");
    printf("-Speed: ");
    scanf("%d", &temp_num);

    buffer_send[1] = 0x08;
    buffer_send[4] = 0x37;

    buffer_send[5] = 0x00;
    buffer_send[6] = 0x00;
    buffer_send[7] = 0x00;
    buffer_send[8] = 0x00;
    buffer_send[9] = 0x00;

    buffer_send[5] = (temp_num & 0xFF); // 거꾸로 들어감.
    buffer_send[6] = ((temp_num >> 8) & 0xFF);
    buffer_send[7] = ((temp_num >> 16) & 0xFF);
    buffer_send[8] = ((temp_num >> 24) & 0xFF);

    printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", buffer_send[0], buffer_send[1], buffer_send[2], buffer_send[3], buffer_send[4], buffer_send[5], buffer_send[6], buffer_send[7], buffer_send[8], buffer_send[9]);
    printf("\n-Direction\n");
    printf("+Jog(1), -Jog(0): ");
    scanf("%x", &buffer_send[9]);

    printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", buffer_send[0], buffer_send[1], buffer_send[2], buffer_send[3], buffer_send[4], buffer_send[5], buffer_send[6], buffer_send[7], buffer_send[8], buffer_send[9]);
    printf("\nend\n");
}
