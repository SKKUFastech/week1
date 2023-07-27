#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3001

extern int client_socket; // 클라이언트 소켓을 외부에서 선언하여 사용합니다.
extern struct sockaddr_in server_addr; // 서버 주소 정보를 외부에서 선언하여 사용합니다.

int UDP_Client_Connect(char *server_ip)
{
    // 소켓 생성
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton 실패");
        exit(EXIT_FAILURE);
    }

    printf("\nFASTECH_UDP_Protocol\n\n");
    // close(client_socket); // 주석 처리된 부분은 소켓을 연결한 후에 연결을 종료하는 코드
    return 0;
}
