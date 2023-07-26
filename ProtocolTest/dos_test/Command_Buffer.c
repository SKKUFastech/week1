#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 258 // 최대 데이터 크기
int FAS_GetboardInfo();
int FAS_ServoEnable();

extern char sync_num;
extern char buffer[BUFFER_SIZE]; // 데이터를 저장할 버퍼

int Command_Buffer(int FrameType)
{
    switch(FrameType){
        case 0x01:

            break;
        case 0x2A:
            FAS_ServoEnable();
            break;
    }
    return 0;
}

int FAS_GetboardInfo()
{
    printf("0x01: FAS_GetboardInfo");
}

int FAS_ServoEnable()
{
    printf("0x2A: FAS_ServoEnable\n");
    printf("ON(1), OFF(0): ");
    scanf("%x", &buffer[5]);
    
    buffer[1] = 0x04;
    buffer[4] = 0x2A;
}
