#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdint.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include "../include/constants.h"
#include "../include/auth.h"
#include "../include/admin_handler.h"
#include "../include/faculty_handler.h"
#include "../include/student_handler.h"

void handle_client(void *arg);
void setup_initial_admin();

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_CLIENTS];
    int thread_index = 0;
    
    // Create data directory if it doesn't exist
    mkdir("./data", 0777);
    
    // Set up initial admin account if it doesn't exist
    setup_initial_admin();
    
    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Academia server started on port %d...\n", PORT);
    
    // Accept and handle client connections
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        printf("New client connected, socket fd: %d\n", client_socket);
        
        // Create a thread to handle the client
        if (pthread_create(&threads[thread_index], NULL, (void *)handle_client, (void *)(intptr_t)client_socket) != 0) {
            perror("pthread_create failed");
            close(client_socket);
        } else {
            // Detach the thread to automatically clean up resources
            pthread_detach(threads[thread_index]);
            thread_index = (thread_index + 1) % MAX_CLIENTS;
        }
    }
    
    // Close the server socket
    close(server_fd);
    
    return 0;
}

void handle_client(void *arg) {
    int client_socket = (int)(intptr_t)arg;
    char buffer[BUFFER_SIZE];
    int choice;
    User user;
    
    // Send welcome message
    sprintf(buffer, "..................Welcome Back to Academia :: Course Registration..................\n"
                    "Login Type\n"
                    "Enter Your Choice { 1.Admin , 2.Faculty, 3. Student }: ");
    write(client_socket, buffer, strlen(buffer));
    
    // Get login type
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    choice = atoi(buffer);
    
    // Handle login
    if (choice == ADMIN || choice == FACULTY || choice == STUDENT) {
        // Get username
        sprintf(buffer, "Username: ");
        write(client_socket, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        char username[MAX_USERNAME];
        strncpy(username, buffer, MAX_USERNAME);
        
        // Get password
        sprintf(buffer, "Password: ");
        write(client_socket, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        char password[MAX_PASSWORD];
        strncpy(password, buffer, MAX_PASSWORD);
        
        // Authenticate user
        int result = authenticate_user(username, password, &user);
        
        if (result == SUCCESS && user.type == choice) {
            // Authentication successful
            sprintf(buffer, "Login successful!\n");
            write(client_socket, buffer, strlen(buffer));
            
            // Handle user based on type
            if (user.type == ADMIN) {
                handle_admin(client_socket, user);
            } else if (user.type == FACULTY) {
                handle_faculty(client_socket, user);
            } else if (user.type == STUDENT) {
                handle_student(client_socket, user);
            }
        } else if (result == INACTIVE) {
            sprintf(buffer, "Your account is inactive. Please contact the administrator.\n");
            write(client_socket, buffer, strlen(buffer));
        } else {
            sprintf(buffer, "Invalid username or password. Please try again.\n");
            write(client_socket, buffer, strlen(buffer));
        }
    } else {
        sprintf(buffer, "Invalid choice. Please try again.\n");
        write(client_socket, buffer, strlen(buffer));
    }
    
    // Close the connection
    close(client_socket);
    printf("Client disconnected, socket fd: %d\n", client_socket);
}

void setup_initial_admin() {
    FILE *file = fopen(ADMIN_FILE, "r");
    
    if (file == NULL) {
        // Admin file doesn't exist, create initial admin
        User admin;
        admin.id = 1;
        strcpy(admin.username, "admin");
        strcpy(admin.password, "admin123");
        strcpy(admin.name, "System Administrator");
        admin.type = ADMIN;
        admin.status = ACTIVE;
        
        add_user(admin);
        printf("Initial admin account created.\n");
    } else {
        fclose(file);
    }
}