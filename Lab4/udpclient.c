#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int choice;

    printf("Options:\n");
    printf("1. Request student details by Registration Number\n");
    printf("2. Request student details by Name\n");
    printf("3. Request marks by Subject Code\n");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &choice);

    // Send the selected option to the server
    sendto(client_socket, &choice, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);

    switch (choice) {
        case 1: // Registration Number
            {
                int regNumber;
                printf("Enter Registration Number: ");
                scanf("%d", &regNumber);
                // Send the registration number to the server
                sendto(client_socket, &regNumber, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);
            }
            break;

        case 2: // Name of the Student
            {
                char name[256];
                printf("Enter Name of the Student: ");
                scanf(" %[^\n]", name);
                // Send the student name to the server
                sendto(client_socket, name, sizeof(name), 0, (struct sockaddr*)&server_addr, addr_len);
            }
            break;

        case 3: // Subject Code
            {
                int subjectCode;
                printf("Enter Subject Code: ");
                scanf("%d", &subjectCode);
                // Send the subject code to the server
                sendto(client_socket, &subjectCode, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);
            }
            break;

        default:
            printf("Invalid choice.\n");
            break;
    }

    // Receive and display the server's response
    ssize_t bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
    if (bytes_received == -1) {
        perror("Receiving failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Server Response:\n%s\n", buffer);

    close(client_socket);

    return 0;
}
