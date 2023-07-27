#include <stdio.h>
#include <stdlib.h>

void Print_Command_List()
{
    system("clear");
    
    printf("\033[16;1H");
    printf("**List**\n");
    printf("0x01: FAS_GetboardInfo\n");
    printf("0x05: FAS_GetMotorInfo\n");
    printf("0x2A: FAS_ServoEnable\n");
    printf("0x31: FAS_MoveStop\n");
    printf("0x33: FAS_MoveOrigin\n");
    // printf("0x58: FAS_MovePause\n");
    printf("0x37: FAS_MoveVelocity\n\n");
    
    printf("\033[1;1H");
    }