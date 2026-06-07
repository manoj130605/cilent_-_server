/*
 * TCP Echo Server
 * Compatible with Windows and Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#define CLOSESOCKET closesocket
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define CLOSESOCKET close
#endif

#define PORT 8080
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

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind");
        CLOSESOCKET(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        CLOSESOCKET(server_fd);
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d...\n", PORT);

    socklen_t client_len = sizeof(client_addr);

    client_fd = accept(server_fd,
                       (struct sockaddr *)&client_addr,
                       &client_len);

    if (client_fd < 0)
    {
        perror("accept");
        CLOSESOCKET(server_fd);
        return EXIT_FAILURE;
    }

    printf("Client connected: %s\n",
           inet_ntoa(client_addr.sin_addr));

    while (1)
    {
        int bytes_received =
            recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("Client: %s\n", buffer);

        send(client_fd,
             buffer,
             (int)strlen(buffer),
             0);
    }

    CLOSESOCKET(client_fd);
    CLOSESOCKET(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}