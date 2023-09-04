#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
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

// Function to send a request for student details by registration number
void requestByRegNumber(int client_socket) {
    int option = 1;
    int regNumber;
    printf("Enter Registration Number: ");
    scanf("%d", &regNumber);
    
    // Send the selected option to the server
    write(client_socket, &option, sizeof(option));
    // Send the registration number to the server
    write(client_socket, &regNumber, sizeof(regNumber));
    
    // Receive and display the server's response
    struct Student student;
    read(client_socket, &student, sizeof(struct Student));
    if (student.regNumber != 0) {
        printf("Server Response:\n");
        printf("Name: %s\n", student.name);
        printf("Residential Address: %s\n", student.address);
        printf("Child PID: %d\n", getpid());
    } else {
        printf("Student not found.\n");
    }
}

// Function to send a request for student details by name
void requestByName(int client_socket) {
    int option = 2;
    char name[256];
    printf("Enter Name of the Student: ");
    scanf(" %[^\n]", name);
    
    // Send the selected option to the server
    write(client_socket, &option, sizeof(option));
    // Send the student name to the server
    write(client_socket, name, sizeof(name));
    
    // Receive and display the server's response
    struct Student student;
    read(client_socket, &student, sizeof(struct Student));
    if (student.regNumber != 0) {
        printf("Server Response:\n");
        printf("Dept: %s\n", student.department);
        printf("Semester: %s\n", student.semester);
        printf("Section: %s\n", student.section);
        printf("Courses: %s\n", student.courses);
        printf("Child PID: %d\n", getpid());
    } else {
        printf("Student not found.\n");
    }
}

// Function to send a request for marks by subject code
void requestBySubjectCode(int client_socket) {
    int option = 3;
    int subjectCode;
    printf("Enter Subject Code: ");
    scanf("%d", &subjectCode);
    
    // Send the selected option to the server
    write(client_socket, &option, sizeof(option));
    // Send the subject code to the server
    write(client_socket, &subjectCode, sizeof(subjectCode));
    
    // Receive and display the server's response
    int marks;
    read(client_socket, &marks, sizeof(int));
    printf("Server Response:\n");
    printf("Marks in Subject: %d\n", marks);
    printf("Child PID: %d\n", getpid());
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    
    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    int choice;
    
    printf("Options:\n");
    printf("1. Request student details by Registration Number\n");
    printf("2. Request student details by Name\n");
    printf("3. Request marks by Subject Code\n");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            requestByRegNumber(client_socket);
            break;
        case 2:
            requestByName(client_socket);
            break;
        case 3:
            requestBySubjectCode(client_socket);
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
    
    close(client_socket);
    
    return 0;
}
