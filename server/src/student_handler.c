#include "../include/student_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_student(int client_socket, User student) {
    char buffer[BUFFER_SIZE];
    int choice;
    
    while (1) {
        // Send menu
        sprintf(buffer, "\n....... Welcome to Student Menu .......\n"
                        "1. View All Courses\n"
                        "2. Enroll (pick) New Course\n"
                        "3. Drop Course\n"
                        "4. View Enrolled Course Details\n"
                        "5. Change Password\n"
                        "6. Logout and Exit\n"
                        "Enter Your Choice: ");
        write(client_socket, buffer, strlen(buffer));
        
        // Get choice
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = 0;
        choice = atoi(buffer);
        
        switch (choice) {
            case 1:
                view_all_courses_handler(client_socket);
                break;
            case 2:
                enroll_course_handler(client_socket, student.id);
                break;
            case 3:
                unenroll_course_handler(client_socket, student.id);
                break;
            case 4:
                view_enrolled_courses_handler(client_socket, student.id);
                break;
            case 5:
                student_change_password_handler(client_socket, student);
                break;
            case 6:
                sprintf(buffer, "Logging out...\n");
                write(client_socket, buffer, strlen(buffer));
                return;
            default:
                sprintf(buffer, "Invalid choice. Please try again.\n");
                write(client_socket, buffer, strlen(buffer));
        }
    }
}

void enroll_course_handler(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    int course_id;
    Course courses[MAX_COURSES];
    int count;
    
    // Get available courses
    int result = get_available_courses(courses, &count);
    
    if (result != SUCCESS || count == 0) {
        sprintf(buffer, "No available courses to enroll.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display available courses
    sprintf(buffer, "\nAvailable Courses:\nID\tCourse ID\tName\t\tAvailable Seats\n");
    write(client_socket, buffer, strlen(buffer));
    
    for (int i = 0; i < count; i++) {
        sprintf(buffer, "%d\t%s\t\t%s\t\t%d\n", 
                courses[i].id, courses[i].course_id, courses[i].name, courses[i].available_seats);
        write(client_socket, buffer, strlen(buffer));
    }
    
    // Get course ID to enroll
    sprintf(buffer, "Enter course ID to enroll (0 to cancel): ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0;
    course_id = atoi(buffer);
    
    if (course_id == 0) {
        sprintf(buffer, "Enrollment canceled.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Enroll in the course
    result = enroll_course(student_id, course_id);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Enrolled in course successfully.\n");
    } else if (result == ALREADY_EXISTS) {
        sprintf(buffer, "You are already enrolled in this course.\n");
    } else if (result == COURSE_FULL) {
        sprintf(buffer, "Course is full. Cannot enroll.\n");
    } else if (result == NOT_FOUND) {
        sprintf(buffer, "Course with ID %d not found.\n", course_id);
    } else {
        sprintf(buffer, "Failed to enroll in course. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void unenroll_course_handler(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    int course_id;
    int courses[MAX_COURSES];
    int count;
    Course course;
    
    // Get enrolled courses
    int result = get_enrolled_courses(student_id, courses, &count);
    
    if (result != SUCCESS || count == 0) {
        sprintf(buffer, "You are not enrolled in any courses.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display enrolled courses
    sprintf(buffer, "\nEnrolled Courses:\nID\tCourse ID\tName\n");
    write(client_socket, buffer, strlen(buffer));
    
    for (int i = 0; i < count; i++) {
        // Get course details
        if (find_course_by_id(courses[i], &course) == SUCCESS) {
            sprintf(buffer, "%d\t%s\t\t%s\n", 
                    course.id, course.course_id, course.name);
            write(client_socket, buffer, strlen(buffer));
        }
    }
    
    // Get course ID to unenroll
    sprintf(buffer, "Enter course ID to drop (0 to cancel): ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0;
    course_id = atoi(buffer);
    
    if (course_id == 0) {
        sprintf(buffer, "Drop canceled.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Unenroll from the course
    result = unenroll_course(student_id, course_id);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Dropped course successfully.\n");
    } else if (result == NOT_ENROLLED) {
        sprintf(buffer, "You are not enrolled in this course.\n");
    } else if (result == NOT_FOUND) {
        sprintf(buffer, "Course with ID %d not found.\n", course_id);
    } else {
        sprintf(buffer, "Failed to drop course. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void view_enrolled_courses_handler(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    int courses[MAX_COURSES];
    int count;
    Course course;
    
    // Get enrolled courses
    int result = get_enrolled_courses(student_id, courses, &count);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Failed to get enrolled courses. Please try again.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display enrolled courses
    sprintf(buffer, "\nEnrolled Courses:\n");
    write(client_socket, buffer, strlen(buffer));
    
    if (count == 0) {
        sprintf(buffer, "You are not enrolled in any courses.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    sprintf(buffer, "ID\tCourse ID\tName\t\tFaculty\n");
    write(client_socket, buffer, strlen(buffer));
    
    for (int i = 0; i < count; i++) {
        // Get course details
        if (find_course_by_id(courses[i], &course) == SUCCESS) {
            Faculty faculty;
            if (find_faculty_by_id(course.faculty_id, &faculty) == SUCCESS) {
                sprintf(buffer, "%d\t%s\t\t%s\t\t%s\n", 
                        course.id, course.course_id, course.name, faculty.name);
            } else {
                sprintf(buffer, "%d\t%s\t\t%s\t\tUnknown\n", 
                        course.id, course.course_id, course.name);
            }
            write(client_socket, buffer, strlen(buffer));
        }
    }
    
    sprintf(buffer, "\nTotal enrolled courses: %d\n", count);
    write(client_socket, buffer, strlen(buffer));
}

void view_all_courses_handler(int client_socket) {
    char buffer[BUFFER_SIZE];
    Course courses[MAX_COURSES];
    int count;
    
    // Get all courses
    int result = get_all_courses(courses, &count);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Failed to get courses. Please try again.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display all courses
    sprintf(buffer, "\nAll Courses:\n");
    write(client_socket, buffer, strlen(buffer));
    
    if (count == 0) {
        sprintf(buffer, "No courses available.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    sprintf(buffer, "ID\tCourse ID\tName\t\tAvailable Seats\tFaculty\n");
    write(client_socket, buffer, strlen(buffer));
    
    for (int i = 0; i < count; i++) {
        Faculty faculty;
        if (find_faculty_by_id(courses[i].faculty_id, &faculty) == SUCCESS) {
            sprintf(buffer, "%d\t%s\t\t%s\t\t%d\t\t%s\n", 
                    courses[i].id, courses[i].course_id, courses[i].name, 
                    courses[i].available_seats, faculty.name);
        } else {
            sprintf(buffer, "%d\t%s\t\t%s\t\t%d\t\tUnknown\n", 
                    courses[i].id, courses[i].course_id, courses[i].name, 
                    courses[i].available_seats);
        }
        write(client_socket, buffer, strlen(buffer));
    }
    
    sprintf(buffer, "\nTotal courses: %d\n", count);
    write(client_socket, buffer, strlen(buffer));
}

void student_change_password_handler(int client_socket, User user) {
    char buffer[BUFFER_SIZE];
    char new_password[MAX_PASSWORD];
    
    // Get new password
    sprintf(buffer, "Enter new password: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(new_password, buffer, MAX_PASSWORD);
    
    // Change password
    int result = change_password(user.id, new_password, user.type);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Password changed successfully.\n");
    } else {
        sprintf(buffer, "Failed to change password. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}