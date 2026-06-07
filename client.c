/*
 * TCP Echo Client
 * Compatible with Windows and Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define CLOSESOCKET closesocket
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define CLOSESOCKET close
#endif

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main(void)
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        return EXIT_FAILURE;
    }
#endif

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET,
                  SERVER_IP,
                  &server_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address\n");
        return EXIT_FAILURE;
    }

    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connect");
        return EXIT_FAILURE;
    }

    printf("Connected to server.\n");

    while (1)
    {
        printf("Enter message: ");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;

        send(sockfd,
             buffer,
             (int)strlen(buffer),
             0);

        int bytes_received =
            recv(sockfd,
                 buffer,
                 BUFFER_SIZE - 1,
                 0);

        if (bytes_received <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("Echo from server: %s\n", buffer);
    }

    CLOSESOCKET(sockfd);

#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}