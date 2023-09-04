#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

// Structure to store student information
struct Student {
    int regNumber;
    char name[256];
    char address[256];
    char department[256];
    char semester[256];
    char section[256];
    char courses[256];
    int marks;
};

// Simulated student database (for demonstration purposes)
struct Student students[] = {
    {1001, "John Doe", "123 Main St", "Computer Science", "3rd", "A", "Programming, Math, Physics", 95},
    {1002, "Jane Smith", "456 Elm St", "Electrical Engineering", "2nd", "B", "Electronics, Circuits, Math", 88},
    {1003, "Bob Johnson", "789 Oak St", "Mechanical Engineering", "4th", "C", "Mechanics, Thermodynamics", 75}
};

int numStudents = sizeof(students) / sizeof(struct Student);

// Function to find a student by registration number
struct Student* findStudentByRegNumber(int regNumber) {
    for (int i = 0; i < numStudents; i++) {
        if (students[i].regNumber == regNumber) {
            return &students[i];
        }
    }
    return NULL; // Student not found
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Receive a request from the client
        ssize_t bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received == -1) {
            perror("Receiving failed");
            continue;
        }

        // Handle the request based on the received data
        int option;
        memcpy(&option, buffer, sizeof(int));

        struct Student* student;
        char response[MAX_BUFFER_SIZE];

        switch (option) {
            case 1: // Registration Number
                {
                    int regNumber;
                    memcpy(&regNumber, buffer + sizeof(int), sizeof(int));
                    student = findStudentByRegNumber(regNumber);
                    if (student != NULL) {
                        // Send student details
                        snprintf(response, sizeof(response), "Name: %s\nResidential Address: %s\n", student->name, student->address);
                        sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                    } else {
                        // Student not found
                        strcpy(response, "Student not found");
                        sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                    }
                }
                break;

            case 2: // Name of the Student
                {
                    char name[256];
                    memcpy(name, buffer + sizeof(int), sizeof(name));
                    // Search for the student by name (for demonstration purposes)
                    student = NULL;
                    for (int i = 0; i < numStudents; i++) {
                        if (strcmp(students[i].name, name) == 0) {
                            student = &students[i];
                            break;
                        }
                    }
                    if (student != NULL) {
                        // Send student details
                        snprintf(response, sizeof(response), "Dept: %s\nSemester: %s\nSection: %s\nCourses: %s\n", student->department, student->semester, student->section, student->courses);
                        sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                    } else {
                        // Student not found
                        strcpy(response, "Student not found");
                        sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                    }
                }
                break;

            case 3: // Subject Code
                {
                    int subjectCode;
                    memcpy(&subjectCode, buffer + sizeof(int), sizeof(int));
                    // Handle subject code request (for demonstration purposes)
                    // In a real application, you would have a database of subject codes and marks
                    // Here, we return a sample mark for demonstration purposes
                    int marks = 0;
                    if (subjectCode >= 0 && subjectCode < numStudents) {
                        marks = students[subjectCode].marks;
                    }
                    // Send marks
                    snprintf(response, sizeof(response), "Marks in Subject: %d\n", marks);
                    sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                }
                break;

            default:
                // Invalid option
                strcpy(response, "Invalid option");
                sendto(server_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
                break;
        }
    }

    close(server_socket);

    return 0;
}
