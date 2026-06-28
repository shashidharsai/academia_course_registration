#ifndef FACULTY_HANDLER_H
#define FACULTY_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "user.h"
#include "course.h"
#include "student_handler.h"

// Handle faculty operations
void handle_faculty(int client_socket, User faculty);

// Faculty operations
void add_course_handler(int client_socket, int faculty_id);
void remove_course_handler(int client_socket, int faculty_id);
void view_course_enrollments_handler(int client_socket, int faculty_id);
void view_offered_courses_handler(int client_socket, int faculty_id);
void update_course_handler(int client_socket, int faculty_id);
void faculty_change_password_handler(int client_socket, User user);

#endif