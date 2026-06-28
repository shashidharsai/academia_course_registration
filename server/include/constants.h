#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_NAME 100
#define MAX_COURSE_ID 10
#define MAX_COURSE_NAME 100
#define MAX_COURSES 20
#define MAX_SEATS 50
#define MAX_ENROLLMENTS 100

// User types
#define ADMIN 1
#define FACULTY 2
#define STUDENT 3

// Status codes
#define SUCCESS 1
#define FAILURE 0
#define ALREADY_EXISTS -1
#define NOT_FOUND -2
#define COURSE_FULL -3
#define NOT_ENROLLED -4
#define ACTIVE 1
#define INACTIVE 0

// File paths
#define ADMIN_FILE "./data/admin.dat"
#define FACULTY_FILE "./data/faculty.dat"
#define STUDENT_FILE "./data/student.dat"
#define COURSE_FILE "./data/course.dat"

#endif