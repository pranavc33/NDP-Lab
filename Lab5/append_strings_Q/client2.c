#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 12345

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection to server failed");
        return EXIT_FAILURE;
    }

    // Send data to the server
    char data[] = "Technology";
    send(client_socket, data, strlen(data), 0);

    // Send the socket address to the server
    char client_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), client_address, INET_ADDRSTRLEN);
    send(client_socket, client_address, strlen(client_address), 0);

    // Close the client socket
    close(client_socket);

    return EXIT_SUCCESS;
}
