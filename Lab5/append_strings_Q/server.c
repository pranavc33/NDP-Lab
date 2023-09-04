#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 2

pthread_mutex_t lock;
int client_count = 0;

void appendToFile(const char *filename, const char *data)
{
    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s", data);
    fclose(file);
}

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    char client_data[1024];
    char client_address[INET_ADDRSTRLEN];

    // Receive data from the client
    ssize_t bytes_received = recv(client_socket, client_data, sizeof(client_data), 0);
    if (bytes_received < 0)
    {
        perror("Error receiving data from client");
        close(client_socket);
        return NULL;
    }
    client_data[bytes_received] = '\0';

    // Get the client's socket address
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_address, INET_ADDRSTRLEN);

    // Append data to the file if it contains the keyword 'Manipal'
    if (strstr(client_data, "Manipal") != NULL)
    {
        pthread_mutex_lock(&lock);
        appendToFile("data.txt", client_data);
        pthread_mutex_unlock(&lock);
    }

    printf("Received from %s: %s\n", client_address, client_data);

    close(client_socket);
    return NULL;
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("Mutex initialization failed");
        return EXIT_FAILURE;
    }

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

        // Handle the client in a new thread
        if (pthread_create(&tid, NULL, handle_client, &client_socket) != 0)
        {
            perror("Thread creation failed");
            continue;
        }

        // Limit the number of clients
        if (++client_count >= MAX_CLIENTS)
        {
            printf("Maximum number of clients reached. Terminating all clients.\n");
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                send(client_socket, "terminate session", strlen("terminate session"), 0);
            }
            break;
        }
    }

    close(server_socket);
    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
