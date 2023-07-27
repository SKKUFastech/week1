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
char SERVER_IP[SERVER_IP_SIZE]; // 서버 IP 주소를 저장하는 변수, "192.168.0.171"과 같이 임시로 설정할 때 사용합니다.
int client_socket; // UDP 클라이언트 소켓을 사용하기 위한 변수
struct sockaddr_in server_addr;

extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
extern ssize_t received_bytes;

int choose_menu();

int main()
{
    int FrameType;     // 사용자로부터 입력받은 프레임 타입을 저장할 변수
    bool ip_ok = true; // 임시로 true(1)로 설정


    while (1)
    {
        if (menu == 1)
        {
            Get_IP_Address(SERVER_IP); // 사용자로부터 서버 IP 주소를 입력받습니다.
            menu = choose_menu(); // 메뉴를 선택하는 함수를 호출하여 선택한 메뉴를 받아옵니다.
            ip_ok = true; // IP 주소가 설정되었음을 표시하기 위해 true로 설정합니다.
        }
        else if (menu == 2)
        {
            system("clear"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함. 화면을 지웁니다.
            Monitor(); // 모니터링을 시작합니다.
            FrameType = Command(); // 사용자로부터 명령을 입력받습니다.
            if (FrameType == 0xFF)
            { // 0xFF 입력시 메인화면으로 이동
                menu = 0;
                continue;
            }
            else if (FrameType == 0xFE)
            { // 0xFE 입력시 명령 리스트 출력 후 메인화면으로 이동
                Print_Command_List();
                menu = 0;
                continue;
            }
            else
            {
                if (ip_ok == true) // IP 주소가 설정되었다면 연결을 한 번만 시도하고 그 이후로는 안되게 합니다.
                {
                    UDP_Client_Connect(SERVER_IP); // 서버에 UDP 클라이언트로 연결합니다.
                    ip_ok = false; // 연결을 했으므로 IP 주소 설정 상태를 false(0)로 변경합니다.
                }
                UDP_Client_Send(FrameType); // 서버로 프레임 타입을 전송합니다.

                sync_num++;
                if (sync_num == 0x254) // sync_num이 254가 되면 다시 1로 복귀합니다.
                    sync_num = 0x01;
            }
        }
        else if (menu == 3)
        {
            break; // 종료 메뉴를 선택하면 반복문을 빠져나와 프로그램을 종료합니다.
        }
        else
        {
            menu = choose_menu(); // 잘못된 메뉴 선택일 경우 메뉴를 다시 선택하도록 합니다.
        }
    }
}

int choose_menu()
{
    int menu;
    system("clear"); 
    printf("MENU\n");
    printf("1. Set IP Address\n");
    printf("2. Command Mode\n");
    printf("3. EXIT\n\n");
    printf("Current IP Address: %s\n\nChoose the Menu: ", SERVER_IP); // 현재 설정된 서버 IP 주소를 표시합니다.
    scanf("%d", &menu); // 사용자로부터 메뉴를 선택받습니다.
    return menu;
}
