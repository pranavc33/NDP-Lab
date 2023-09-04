// dns_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define DATABASE_FILE "database.txt"

struct DNSRecord {
    char domain[100];
    char ip[16];
};

void loadDatabase(struct DNSRecord *records, int *recordCount) {
    FILE *file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        perror("Error opening database file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while (fscanf(file, "%s %s", records[count].domain, records[count].ip) != EOF) {
        count++;
    }

    *recordCount = count;
    fclose(file);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    char buffer[100];
    struct DNSRecord records[100];
    int recordCount;

    loadDatabase(records, &recordCount);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
        if (new_socket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, sizeof(buffer));

        // Search for the IP address in the database
        int found = 0;
        for (int i = 0; i < recordCount; i++) {
            if (strcmp(buffer, records[i].domain) == 0) {
                found = 1;
                write(new_socket, records[i].ip, strlen(records[i].ip));
                break;
            }
        }

        if (!found) {
            char notFoundMsg[] = "Domain not found";
            write(new_socket, notFoundMsg, strlen(notFoundMsg));
        }

        close(new_socket);
    }

    return 0;
}
