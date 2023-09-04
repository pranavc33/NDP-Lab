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
    char buffer[256];
    char str1[256], str2[256];

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

    // Receive and display the date and time from the server
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server Date and Time: %s\n", buffer);

    // Prompt the user to enter two strings
    printf("Enter the first string: ");
    fgets(str1, sizeof(str1), stdin);
    str1[strcspn(str1, "\n")] = '\0'; // Remove newline character

    printf("Enter the second string: ");
    fgets(str2, sizeof(str2), stdin);
    str2[strcspn(str2, "\n")] = '\0'; // Remove newline character

    // Send the strings to the server
    send(client_socket, str1, strlen(str1), 0);
    send(client_socket, str2, strlen(str2), 0);

    // Receive and display the result from the server
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Result from Server: %s\n", buffer);

    // Close the client socket
    close(client_socket);

    return EXIT_SUCCESS;
}
