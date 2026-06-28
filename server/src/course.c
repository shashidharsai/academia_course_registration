#include "../include/course.h"
#include "../include/file_lock.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int add_course(Course course) {
    int fd;
    Course temp;
    
    // Create directory if it doesn't exist
    mkdir("./data", 0777);
    
    // Open file in read-write mode, create if it doesn't exist
    fd = open(COURSE_FILE, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open course file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Check if course ID already exists
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0) {
        if (strcmp(temp.course_id, course.course_id) == 0 && temp.status == ACTIVE) {
            release_lock(fd, 0, 0);
            close(fd);
            return ALREADY_EXISTS;
        }
    }
    
    // Generate a new ID
    int max_id = 0;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0) {
        if (temp.id > max_id) {
            max_id = temp.id;
        }
    }
    course.id = max_id + 1;
    course.available_seats = course.max_seats;
    course.status = ACTIVE;
    
    // Write the new course to the end of the file
    lseek(fd, 0, SEEK_END);
    write(fd, &course, sizeof(Course));
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int find_course_by_id(int id, Course *course) {
    int fd;
    Course temp;
    
    // Open the file
    fd = open(COURSE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open course file");
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the course ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0) {
        if (temp.id == id) {
            *course = temp;
            release_lock(fd, 0, 0);
            close(fd);
            return SUCCESS;
        }
    }
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return NOT_FOUND;
}

int update_course(Course course) {
    int fd;
    Course temp;
    
    // Open the file
    fd = open(COURSE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open course file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the course ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0) {
        if (temp.id == course.id) {
            // Move back to update the record
            lseek(fd, -sizeof(Course), SEEK_CUR);
            write(fd, &course, sizeof(Course));
            release_lock(fd, 0, 0);
            close(fd);
            return SUCCESS;
        }
    }
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return NOT_FOUND;
}

int remove_course(int id) {
    Course course;
    
    // Find the course
    if (find_course_by_id(id, &course) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Set the course as inactive
    course.status = INACTIVE;
    
    // Update the course
    return update_course(course);
}

int get_all_courses(Course *courses, int *count) {
    int fd;
    Course temp;
    int i = 0;
    
    // Open the file
    fd = open(COURSE_FILE, O_RDONLY);
    if (fd == -1) {
        *count = 0;
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        *count = 0;
        return FAILURE;
    }
    
    // Read all active courses
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0 && i < MAX_COURSES) {
        if (temp.status == ACTIVE) {
            courses[i++] = temp;
        }
    }
    
    *count = i;
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int get_available_courses(Course *courses, int *count) {
    int fd;
    Course temp;
    int i = 0;
    
    // Open the file
    fd = open(COURSE_FILE, O_RDONLY);
    if (fd == -1) {
        *count = 0;
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        *count = 0;
        return FAILURE;
    }
    
    // Read all active courses with available seats
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Course)) > 0 && i < MAX_COURSES) {
        if (temp.status == ACTIVE && temp.available_seats > 0) {
            courses[i++] = temp;
        }
    }
    
    *count = i;
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int get_course_enrollments(int course_id, int *student_ids, int *count) {
    int fd;
    Student student;
    int i = 0;
    
    // Open the student file
    fd = open(STUDENT_FILE, O_RDONLY);
    if (fd == -1) {
        *count = 0;
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        *count = 0;
        return FAILURE;
    }
    
    // Read all students enrolled in the course
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &student, sizeof(Student)) > 0 && i < MAX_ENROLLMENTS) {
        for (int j = 0; j < student.enrolled_count; j++) {
            if (student.enrolled_courses[j] == course_id) {
                student_ids[i++] = student.id;
                break;
            }
        }
    }
    
    *count = i;
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int enroll_student(int course_id, int student_id) {
    return enroll_course(student_id, course_id);
}

int unenroll_student(int course_id, int student_id) {
    return unenroll_course(student_id, course_id);
}

int is_enrolled(int course_id, int student_id) {
    Student student;
    
    // Find the student
    if (find_student_by_id(student_id, &student) != SUCCESS) {
        return FAILURE;
    }
    
    // Check if student is enrolled in the course
    for (int i = 0; i < student.enrolled_count; i++) {
        if (student.enrolled_courses[i] == course_id) {
            return SUCCESS;
        }
    }
    
    return NOT_ENROLLED;
}