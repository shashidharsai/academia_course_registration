#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/client.h"

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }
    
    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection Failed. Server might be down.\n");
        return -1;
    }
    
    // Communication loop
    while (1) {
        // Receive data from server
        memset(buffer, 0, sizeof(buffer));
        int read_size = read(client_socket, buffer, BUFFER_SIZE);
        
        if (read_size <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        // Print server message
        printf("%s", buffer);
        
        // Get input from user
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Send data to server
        write(client_socket, buffer, strlen(buffer));
        
        // Check if it's an exit command
        if (strncmp(buffer, "6", 1) == 0 || strncmp(buffer, "9", 1) == 0) {
            // Receive the goodbye message
            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            break;
        }
    }
    
    close(client_socket);
    return 0;
}