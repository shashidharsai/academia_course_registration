#ifndef ADMIN_HANDLER_H
#define ADMIN_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "user.h"
#include "course.h"
#include "file_lock.h"

// Handle admin operations
void handle_admin(int client_socket, User admin);

// Admin operations
void add_student_handler(int client_socket);
void add_faculty_handler(int client_socket);
void activate_deactivate_student_handler(int client_socket, int status);
void update_user_details_handler(int client_socket, int type);
void view_student_handler(int client_socket);
void view_faculty_handler(int client_socket);

#endif