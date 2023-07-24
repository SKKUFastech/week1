#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Get_IP_Address.h"
#include "UDP_Client.h"
// #include <arpa/inet.h>

#define PORT 3001
#define BUFFER_SIZE 258
#define SERVER_IP_SIZE 15

int sync_number;
int menu = 0;

// int Get_IP_Address();
int choose_menu();

int main()
{
    char SERVER_IP[SERVER_IP_SIZE];
    int temp_menu = 0;

    while (1)
    {
        menu = temp_menu;

        if (menu == 1)
        {
            Get_IP_Address(SERVER_IP);
            printf("\n%s\n", SERVER_IP);
            temp_menu = choose_menu();
        }
        else if (menu == 2)
        {
            system("cls"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
            UDP_Client(SERVER_IP);
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
    printf("\n**ProtocolTest for PE**\n\n");
    printf("MENU\n");
    printf("1. Set IP Address\n");
    printf("2. Command Mode\n\nType the number: ");

    scanf("%d", &menu);
    return menu;
}
