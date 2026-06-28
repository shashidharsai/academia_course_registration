#ifndef STUDENT_HANDLER_H
#define STUDENT_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "user.h" 
#include "course.h"
#include "file_lock.h"

// Handle student operations
void handle_student(int client_socket, User student);

// Student operations
void enroll_course_handler(int client_socket, int student_id);
void unenroll_course_handler(int client_socket, int student_id);
void view_enrolled_courses_handler(int client_socket, int student_id);
void view_all_courses_handler(int client_socket);
void student_change_password_handler(int client_socket, User user);

#endif