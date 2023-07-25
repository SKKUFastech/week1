#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Get_IP_Address.h"
#include "UDP_Client.h"
#include "Command.h"

// #include <arpa/inet.h>

#define PORT 3001
#define BUFFER_SIZE 258
#define SERVER_IP_SIZE 14

char sync_num=0x01;
int menu = 0;
char SERVER_IP[SERVER_IP_SIZE]; //="192.168.0.171"

int choose_menu();
// void SetConsoleView();



int main()
{
    unsigned int command, 
    FrameType; // 사용자로부터 입력받은 프레임 타입을 저장할 변수;
    
    int temp_menu = 1;
    SetConsoleView();

    while (1)
    {
        menu = temp_menu;

        if (menu == 1)
        {
            Get_IP_Address(SERVER_IP);
            temp_menu = choose_menu();
        }
        else if (menu == 2)
        {
            system("cls"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
            FrameType=Command();
            UDP_Client(SERVER_IP, FrameType);
            sync_num++;
            if (sync_num==0x255) sync_num=0x01;
        }
        else
        {
            temp_menu = choose_menu();
        }
    }
}

int choose_menu()
{
    int menu;
    system("cls"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
    printf("MENU\n");
    printf("1. Set IP Address\n");
    printf("2. Command Mode\n\n");
    printf("Current IP Address: %s\n\nType the number: ", SERVER_IP);
    scanf("%d", &menu);
    return menu;
}

// void SetConsoleView(){ //리눅스에선 작동안함.
//     system("mode con:cols=100 lines=30");
//     system("title ProtocolTest for PE by FASTECH");
// }