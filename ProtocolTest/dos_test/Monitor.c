#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 258

extern ssize_t received_bytes;
extern char buffer_send[BUFFER_SIZE]; // 보낼 데이터를 저장할 버퍼
extern char buffer_rcv[BUFFER_SIZE];  // 받을 데이터를 저장할 버퍼

void Monitor()
{
    printf("\033[13;1H");
    printf("**Monitor**\n");
    printf("\033[15;1H");
    printf("RaspberryPi: ");
    for (ssize_t i = 0; i < buffer_send[1] + 2; i++)
    {
        printf("%02X ", (unsigned char)buffer_send[i]);
    }
    // Print the received data in hexadecimal format
    printf("\n\nServer: ");
    for (ssize_t i = 0; i < received_bytes; i++)
    {
        printf("%02X ", (unsigned char)buffer_rcv[i]);
    }
    printf("\n");
    if (buffer_rcv[5]==1) printf("Response: OK\n");

    //hex to text



    printf("\033[1;1H");
}