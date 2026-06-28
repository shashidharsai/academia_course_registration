#include "../include/faculty_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void handle_faculty(int client_socket, User faculty) {
    char buffer[BUFFER_SIZE];
    int choice;
    
    while (1) {
        // Send menu
        sprintf(buffer, "\n....... Welcome to Faculty Menu .......\n"
                        "1. View Offering Courses\n"
                        "2. Add New Course\n"
                        "3. Remove Course from Catalog\n"
                        "4. Update Course Details\n"
                        "5. Change Password\n"
                        "6. Logout and Exit\n"
                        "Enter Your Choice: ");
        write(client_socket, buffer, strlen(buffer));
        
        // Get choice
        memset(buffer, 0, sizeof(buffer));
        read(client_socket, buffer, sizeof(buffer));
        choice = atoi(buffer);
        
        switch (choice) {
            case 1:
                view_offered_courses_handler(client_socket, faculty.id);
                break;
            case 2:
                add_course_handler(client_socket, faculty.id);
                break;
            case 3:
                remove_course_handler(client_socket, faculty.id);
                break;
            case 4:
                update_course_handler(client_socket, faculty.id);
                break;
            case 5:
                faculty_change_password_handler(client_socket, faculty);
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

void add_course_handler(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    
    // Get course details
    sprintf(buffer, "Enter course ID (e.g., CS101): ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(course.course_id, buffer, MAX_COURSE_ID);
    
    sprintf(buffer, "Enter course name: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strncpy(course.name, buffer, MAX_COURSE_NAME);
    
    sprintf(buffer, "Enter maximum seats: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course.max_seats = atoi(buffer);
    
    // Set faculty ID
    course.faculty_id = faculty_id;
    
    // Add the course
    int result = add_course(course);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Course added successfully. ID: %d\n", course.id);
    } else if (result == ALREADY_EXISTS) {
        sprintf(buffer, "Course ID already exists. Please try again.\n");
    } else {
        sprintf(buffer, "Failed to add course. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void remove_course_handler(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    int course_id;
    Course course;
    
    // Display courses offered by the faculty
    view_offered_courses_handler(client_socket, faculty_id);
    
    // Get course ID to remove
    sprintf(buffer, "Enter course ID to remove: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);
    
    // Find the course
    int result = find_course_by_id(course_id, &course);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Course with ID %d not found.\n", course_id);
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Check if the course belongs to the faculty
    if (course.faculty_id != faculty_id) {
        sprintf(buffer, "You are not authorized to remove this course.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Remove the course
    result = remove_course(course_id);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Course removed successfully.\n");
    } else {
        sprintf(buffer, "Failed to remove course. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void view_course_enrollments_handler(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    int course_id;
    Course course;
    int student_ids[MAX_ENROLLMENTS];
    int count;
    Student student;
    
    // Display courses offered by the faculty
    view_offered_courses_handler(client_socket, faculty_id);
    
    // Get course ID
    sprintf(buffer, "Enter course ID to view enrollments: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);
    
    // Find the course
    int result = find_course_by_id(course_id, &course);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Course with ID %d not found.\n", course_id);
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Check if the course belongs to the faculty
    if (course.faculty_id != faculty_id) {
        sprintf(buffer, "You are not authorized to view enrollments for this course.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Get course enrollments
    result = get_course_enrollments(course_id, student_ids, &count);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Failed to get enrollments. Please try again.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display enrollments
    sprintf(buffer, "\nEnrollments for %s (%s):\n", course.course_id, course.name);
    write(client_socket, buffer, strlen(buffer));
    
    if (count == 0) {
        sprintf(buffer, "No students enrolled in this course.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    sprintf(buffer, "ID\tName\n");
    write(client_socket, buffer, strlen(buffer));
    
    for (int i = 0; i < count; i++) {
        // Find student details
        if (find_student_by_id(student_ids[i], &student) == SUCCESS) {
            sprintf(buffer, "%d\t%s\n", student.id, student.name);
            write(client_socket, buffer, strlen(buffer));
        }
    }
    
    sprintf(buffer, "\nTotal enrollments: %d\n", count);
    write(client_socket, buffer, strlen(buffer));
}

void view_offered_courses_handler(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    int fd;
    Course course;
    int found = 0;
    
    // Open the course file
    fd = open(COURSE_FILE, O_RDONLY);
    if (fd == -1) {
        sprintf(buffer, "No courses found.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        sprintf(buffer, "Failed to access course records.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Read and display courses offered by the faculty
    sprintf(buffer, "\nOffered Courses:\nID\tCourse ID\tName\t\tMax Seats\tAvailable Seats\tStatus\n");
    write(client_socket, buffer, strlen(buffer));
    
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.faculty_id == faculty_id) {
            sprintf(buffer, "%d\t%s\t\t%s\t\t%d\t\t%d\t\t%s\n", 
                    course.id, course.course_id, course.name, 
                    course.max_seats, course.available_seats,
                    course.status == ACTIVE ? "Active" : "Inactive");
            write(client_socket, buffer, strlen(buffer));
            found = 1;
        }
    }
    
    if (!found) {
        sprintf(buffer, "You haven't offered any courses yet.\n");
        write(client_socket, buffer, strlen(buffer));
    }
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
}

void update_course_handler(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    int course_id;
    Course course;
    
    // Display courses offered by the faculty
    view_offered_courses_handler(client_socket, faculty_id);
    
    // Get course ID to update
    sprintf(buffer, "Enter course ID to update: ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course_id = atoi(buffer);
    
    // Find the course
    int result = find_course_by_id(course_id, &course);
    
    if (result != SUCCESS) {
        sprintf(buffer, "Course with ID %d not found.\n", course_id);
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Check if the course belongs to the faculty
    if (course.faculty_id != faculty_id) {
        sprintf(buffer, "You are not authorized to update this course.\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }
    
    // Display current details
    sprintf(buffer, "Current details:\n"
                    "Course ID: %s\n"
                    "Name: %s\n"
                    "Max Seats: %d\n"
                    "Available Seats: %d\n"
                    "Status: %s\n\n"
                    "Enter new name (or press enter to keep current): ", 
                    course.course_id, course.name, course.max_seats, 
                    course.available_seats, course.status == ACTIVE ? "Active" : "Inactive");
    write(client_socket, buffer, strlen(buffer));
    
    // Get new name
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    if (strlen(buffer) > 0) {
        strncpy(course.name, buffer, MAX_COURSE_NAME);
    }
    
    // Get new max seats
    sprintf(buffer, "Enter new max seats (or press enter to keep current): ");
    write(client_socket, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    if (strlen(buffer) > 0) {
        int new_max_seats = atoi(buffer);
        int enrolled = course.max_seats - course.available_seats;
        
        if (new_max_seats < enrolled) {
            sprintf(buffer, "Cannot reduce max seats below the number of enrolled students (%d).\n", enrolled);
            write(client_socket, buffer, strlen(buffer));
        } else {
            course.available_seats += (new_max_seats - course.max_seats);
            course.max_seats = new_max_seats;
        }
    }
    
    // Update the course
    result = update_course(course);
    
    if (result == SUCCESS) {
        sprintf(buffer, "Course updated successfully.\n");
    } else {
        sprintf(buffer, "Failed to update course. Please try again.\n");
    }
    
    write(client_socket, buffer, strlen(buffer));
}

void faculty_change_password_handler(int client_socket, User user) {
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