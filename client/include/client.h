#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void display_main_menu();
void handle_login();
void handle_admin_menu(int client_socket);
void handle_faculty_menu(int client_socket);
void handle_student_menu(int client_socket);

#endif