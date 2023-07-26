// Get_IP_Address.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Get_IP_Address(char* server_ip)
{
    system("clear"); // 윈도우에서는 cls, 우분투에서는 clear로 바꿔야함.
    printf("Please enter the IP address.(ex:192.168.0.171): ");
    if (scanf("%14s", server_ip) != 1)
    {
        printf("An input error has occurred.\n");
        exit(EXIT_FAILURE);
    }
}