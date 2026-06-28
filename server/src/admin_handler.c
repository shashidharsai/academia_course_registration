#include "../include/admin_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_admin(int client_socket, User admin) {
    char buffer[BUFFER_SIZE];
    int choice;
    
    while (1) {
        // Send menu
        sprintf(buffer, "\n....... Welcome to Admin Menu .......\n"
                        "1. Add Student\n"
                        "2. View Student Details\n"
                        "3. Add Faculty\n"
                        "4. View Faculty Details\n"
                        "5. Activate Student\n"
                        "6. Block Student\n"
                        "7. Modify Student Details\n"
                        "8. Modify Faculty Details\n"
                        "9. Logout and Exit\n"
                        "Enter Your Choice: ");
        write(client_socket, buffer, strlen(buffer));
        
        // Get choice
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        buffer[strcspn(buffer, "\n")] = 0;
        choice = atoi(buffer);
        
        switch (choice) {
            case 1:
                add_student_handler(client_socket);
                break;
            case 2:
                view_student_handler(client_socket);
                break;
            case 3:
                add_faculty_handler(client_socket);
                break;
            case 4:
                view_faculty_handler(client_socket);
                break;
            case 5:
                activate_deactivate_student_handler(client_socket, ACTIVE);
                break;
            case 6:
                activate_deactivate_student_handler(client_socket, INACTIVE);
                break;
            case 7:
                update_user_details_handler(client_socket, STUDENT);
                break;
            case 8:
                update_user_details_handler(client_socket, FACULTY);
                break;
            case 9:
                sprintf(buffer, "Logging out...\n");
                write(client_socket, buffer, strlen(buffer));
                return;
            default:
                sprintf(buffer, "Invalid choice. Please try again.\n");
                write(client_socket, buffer, strlen(buffer));
        }
    }
}

void add_student_handler(int client_socket) {
    char buffer[BUFFER_SIZE];
    Student student;
    
    // Get student details
    sprintf(buffer, "Enter student name: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(student.name, buffer, MAX_NAME);
    
    sprintf(buffer, "Enter username: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(student.username, buffer, MAX_USERNAME);
    
    sprintf(buffer, "Enter password: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(student.password, buffer, MAX_PASSWORD);
    
    // Add the student
    int result = add_student(student);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Student added successfully. ID: %d\n", student.id);
    } else if (result == ALREADY_EXISTS) {
        sprintf(buffer, "Username already exists. Please try again.\n");
    } else {
        sprintf(buffer, "Failed to add student. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void add_faculty_handler(int client_socket) {
    char buffer[BUFFER_SIZE];
    Faculty faculty;
    
    // Get faculty details
    sprintf(buffer, "Enter faculty name: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(faculty.name, buffer, MAX_NAME);
    
    sprintf(buffer, "Enter department: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(faculty.department, buffer, MAX_NAME);
    
    sprintf(buffer, "Enter username: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(faculty.username, buffer, MAX_USERNAME);
    
    sprintf(buffer, "Enter password: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(faculty.password, buffer, MAX_PASSWORD);
    
    // Add the faculty
    int result = add_faculty(faculty);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Faculty added successfully. ID: %d\n", faculty.id);
    } else if (result == ALREADY_EXISTS) {
        sprintf(buffer, "Username already exists. Please try again.\n");
    } else {
        sprintf(buffer, "Failed to add faculty. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void activate_deactivate_student_handler(int client_socket, int status) {
    char buffer[BUFFER_SIZE];
    int student_id;
    
    // Get student ID
    sprintf(buffer, "Enter student ID: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0;
    student_id = atoi(buffer);
    
    // Activate/deactivate the student
    int result = activate_deactivate_user(student_id, status, STUDENT);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Student %s successfully.\n", status == ACTIVE ? "activated" : "blocked");
    } else if (result == NOT_FOUND) {
        sprintf(buffer, "Student with ID %d not found.\n", student_id);
    } else {
        sprintf(buffer, "Failed to %s student. Please try again.\n", status == ACTIVE ? "activate" : "block");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void update_user_details_handler(int client_socket, int type) {
    char buffer[BUFFER_SIZE];
    int user_id;
    User user;
    
    // Get user ID
    sprintf(buffer, "Enter %s ID: ", type == STUDENT ? "student" : "faculty");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0;
    user_id = atoi(buffer);
    
    // Find the user
    int result = find_user_by_id(user_id, &user, type);
    
    if (result != SUCCESS) {
        sprintf(buffer, "%s with ID %d not found.\n", type == STUDENT ? "Student" : "Faculty", user_id);
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display current details
    sprintf(buffer, "Current details:\n"
                    "Name: %s\n"
                    "Username: %s\n"
                    "Status: %s\n\n"
                    "Enter new name (or press enter to keep current): ", 
                    user.name, user.username, user.status == ACTIVE ? "Active" : "Blocked");
    write(client_socket, buffer, strlen(buffer));
    
    // Get new name
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    if (strlen(buffer) > 0) {
        strncpy(user.name, buffer, MAX_NAME);
    }
    
    // Update the user
    result = update_user(user);
    
    if (result == SUCCESS) {
        sprintf(buffer, "%s details updated successfully.\n", type == STUDENT ? "Student" : "Faculty");
    } else {
        sprintf(buffer, "Failed to update %s details. Please try again.\n", type == STUDENT ? "student" : "faculty");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void view_student_handler(int client_socket) {
    char buffer[BUFFER_SIZE];
    int fd;
    Student student;
    
    // Open the student file
    fd = open(STUDENT_FILE, O_RDONLY);
    if (fd == -1) {
        sprintf(buffer, "No students found.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        sprintf(buffer, "Failed to access student records.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Read and display all students
    sprintf(buffer, "\nStudent Details:\nID\tName\t\tUsername\tStatus\n");
    write(client_socket, buffer, strlen(buffer));
    
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &student, sizeof(Student)) > 0) {
        sprintf(buffer, "%d\t%s\t\t%s\t\t%s\n", 
                student.id, student.name, student.username, 
                student.status == ACTIVE ? "Active" : "Blocked");
        write(client_socket, buffer, strlen(buffer));
    }
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
}

void view_faculty_handler(int client_socket) {
    char buffer[BUFFER_SIZE];
    int fd;
    Faculty faculty;
    
    // Open the faculty file
    fd = open(FACULTY_FILE, O_RDONLY);
    if (fd == -1) {
        sprintf(buffer, "No faculty found.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        sprintf(buffer, "Failed to access faculty records.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Read and display all faculty
    sprintf(buffer, "\nFaculty Details:\nID\tName\t\tUsername\tDepartment\tStatus\n");
    write(client_socket, buffer, strlen(buffer));
    
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &faculty, sizeof(Faculty)) > 0) {
        sprintf(buffer, "%d\t%s\t\t%s\t\t%s\t\t%s\n", 
                faculty.id, faculty.name, faculty.username, faculty.department,
                faculty.status == ACTIVE ? "Active" : "Blocked");
        write(client_socket, buffer, strlen(buffer));
    }
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
}