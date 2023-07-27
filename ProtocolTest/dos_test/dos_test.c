#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "Get_IP_Address.h"
#include "UDP_Client_Connect.h"
#include "UDP_Client_Send.h"
#include "Command.h"
#include "Print_Command_List.h"
#include "Monitor.h"

#define PORT 3001
#define BUFFER_SIZE 258
#define SERVER_IP_SIZE 14

char sync_num = 0x03;
int menu = 0;
char SERVER_IP[SERVER_IP_SIZE] = "192.168.0.171"; //="192.168.0.171" 임시로 설정해놓을 때 사용
int client_socket;                                // udp_client 사용
struct sockaddr_in server_addr;

extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
extern ssize_t received_bytes;

int choose_menu();
// void SetConsoleView();

int main()
{
    int FrameType;     // 사용자로부터 입력받은 프레임 타입을 저장할 변수;l
    bool ip_ok = true; // 임시로 1로 설정
    // SetConsoleView();

    while (1)
    {
        if (menu == 1)
        {
            Get_IP_Address(SERVER_IP);
            menu = choose_menu();
            ip_ok = true;
        }
        else if (menu == 2)
        {
            system("clear");
            Monitor();
            FrameType = Command();
            if (FrameType == 0xFF)
            { // ff 입력시 메인화면으로 이동
                menu = 0;
                continue;
            }
            else if (FrameType == 0xFE)
            { // ff 입력시 메인화면으로 이동
                Print_Command_List();
                menu = 0;
                continue;
            }

            else
            {
                if (ip_ok == true) // ip설정되면 연결 한번만하고 안되게 하기.
                {
                    UDP_Client_Connect(SERVER_IP);
                    ip_ok = false;
                }
                UDP_Client_Send(FrameType);

                sync_num++;
                if (sync_num == 0x254) // sync_num이 254되면 다시 1로 복귀
                    sync_num = 0x01;
            }
        }
        else if (menu == 3)
        {
            break;
        }
        else
        {
            menu = choose_menu();
        }
    }
}

int choose_menu()
{
    int menu;
    system("clear"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
    printf("MENU\n");
    printf("1. Set IP Address\n");
    printf("2. Command Mode\n");
    printf("3. EXIT\n\n");
    printf("Current IP Address: %s\n\nChoose the Menu: ", SERVER_IP);
    scanf("%d", &menu);
    return menu;
}

// void SetConsoleView(){ //리눅스에선 작동안함.
//     system("mode con:cols=100 lines=30");
//     system("title ProtocolTest for PE by FASTECH");
// }