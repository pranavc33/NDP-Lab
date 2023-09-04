#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 12345

int isAnagram(const char *str1, const char *str2)
{
    int count[26] = {0};

    if (strlen(str1) != strlen(str2))
    {
        return 0;
    }

    for (int i = 0; str1[i] != '\0'; i++)
    {
        count[str1[i] - 'a']++;
        count[str2[i] - 'a']--;
    }

    for (int i = 0; i < 26; i++)
    {
        if (count[i] != 0)
        {
            return 0;
        }
    }

    return 1;
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[256];
    char client_address[INET_ADDRSTRLEN];
    time_t current_time;
    struct tm *time_info;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Binding failed");
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0)
    {
        perror("Listening failed");
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        // Accept a new client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            perror("Accepting client connection failed");
            continue;
        }

        // Get the client's socket address
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_address, INET_ADDRSTRLEN);

        // Get current date and time
        time(&current_time);
        time_info = localtime(&current_time);

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

        // Send date and time to the client
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive two strings from the client
        char str1[256], str2[256];
        recv(client_socket, str1, sizeof(str1), 0);
        recv(client_socket, str2, sizeof(str2), 0);

        // Check if the strings are anagrams
        int result = isAnagram(str1, str2);

        // Send the result to the client
        if (result)
        {
            send(client_socket, "The strings are anagrams.", strlen("The strings are anagrams."), 0);
        }
        else
        {
            send(client_socket, "The strings are not anagrams.", strlen("The strings are not anagrams."), 0);
        }

        printf("Connected to %s at %s\n", client_address, buffer);

        // Close the client socket
        close(client_socket);
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
