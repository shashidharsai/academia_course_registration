#include "../include/user.h"
#include "../include/file_lock.h"
#include "../include/course.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int add_user(User user) {
    int fd;
    User temp;
    
    // Check user type and open appropriate file
    char *file_path;
    if (user.type == ADMIN) {
        file_path = ADMIN_FILE;
    } else if (user.type == FACULTY) {
        file_path = FACULTY_FILE;
    } else if (user.type == STUDENT) {
        file_path = STUDENT_FILE;
    } else {
        return FAILURE;
    }
    
    // Create directory if it doesn't exist
    mkdir("./data", 0777);
    
    // Open file in read-write mode, create if it doesn't exist
    fd = open(file_path, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Check if username already exists
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(User)) > 0) {
        if (strcmp(temp.username, user.username) == 0) {
            release_lock(fd, 0, 0);
            close(fd);
            return ALREADY_EXISTS;
        }
    }
    
    // Generate a new ID
    int max_id = 0;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(User)) > 0) {
        if (temp.id > max_id) {
            max_id = temp.id;
        }
    }
    user.id = max_id + 1;
    
    // Write the new user to the end of the file
    lseek(fd, 0, SEEK_END);
    write(fd, &user, sizeof(User));
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int find_user_by_username(char *username, User *user) {
    int fd;
    User temp;
    int found = 0;
    
    // Try each file type
    char *file_paths[] = {ADMIN_FILE, FACULTY_FILE, STUDENT_FILE};
    int types[] = {ADMIN, FACULTY, STUDENT};
    
    for (int i = 0; i < 3; i++) {
        fd = open(file_paths[i], O_RDONLY);
        if (fd == -1) {
            continue;  // Skip if file doesn't exist
        }
        
        // Set a read lock on the file
        if (set_read_lock(fd, 0, 0) == -1) {
            perror("Failed to set read lock");
            close(fd);
            continue;
        }
        
        // Search for the username
        lseek(fd, 0, SEEK_SET);
        while (read(fd, &temp, sizeof(User)) > 0) {
            if (strcmp(temp.username, username) == 0) {
                *user = temp;
                found = 1;
                break;
            }
        }
        
        // Release the lock
        release_lock(fd, 0, 0);
        close(fd);
        
        if (found) {
            return SUCCESS;
        }
    }
    
    return NOT_FOUND;
}

int find_user_by_id(int id, User *user, int type) {
    int fd;
    User temp;
    
    // Determine file path based on user type
    char *file_path;
    if (type == ADMIN) {
        file_path = ADMIN_FILE;
    } else if (type == FACULTY) {
        file_path = FACULTY_FILE;
    } else if (type == STUDENT) {
        file_path = STUDENT_FILE;
    } else {
        return FAILURE;
    }
    
    // Open the file
    fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the user ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(User)) > 0) {
        if (temp.id == id) {
            *user = temp;
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

int authenticate_user(char *username, char *password, User *user) {
    // Find the user
    if (find_user_by_username(username, user) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Check password
    if (strcmp(user->password, password) != 0) {
        return FAILURE;
    }
    
    // Check if user is active
    if (user->status != ACTIVE) {
        return INACTIVE;
    }
    
    return SUCCESS;
}

int update_user(User user) {
    int fd;
    User temp;
    
    // Determine file path based on user type
    char *file_path;
    if (user.type == ADMIN) {
        file_path = ADMIN_FILE;
    } else if (user.type == FACULTY) {
        file_path = FACULTY_FILE;
    } else if (user.type == STUDENT) {
        file_path = STUDENT_FILE;
    } else {
        return FAILURE;
    }
    
    // Open the file
    fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the user ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(User)) > 0) {
        if (temp.id == user.id) {
            // Move back to update the record
            lseek(fd, -sizeof(User), SEEK_CUR);
            write(fd, &user, sizeof(User));
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

int change_password(int id, char *new_password, int type) {
    User user;
    
    // Find the user
    if (find_user_by_id(id, &user, type) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Update the password
    strncpy(user.password, new_password, MAX_PASSWORD);
    
    // Save the updated user
    return update_user(user);
}

int activate_deactivate_user(int id, int status, int type) {
    User user;
    
    // Find the user
    if (find_user_by_id(id, &user, type) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Update the status
    user.status = status;
    
    // Save the updated user
    return update_user(user);
}

int add_student(Student student) {
    int fd;
    Student temp;
    
    // Create directory if it doesn't exist
    mkdir("./data", 0777);
    
    // Open file in read-write mode, create if it doesn't exist
    fd = open(STUDENT_FILE, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open student file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Check if username already exists
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Student)) > 0) {
        if (strcmp(temp.username, student.username) == 0) {
            release_lock(fd, 0, 0);
            close(fd);
            return ALREADY_EXISTS;
        }
    }
    
    // Generate a new ID
    int max_id = 0;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Student)) > 0) {
        if (temp.id > max_id) {
            max_id = temp.id;
        }
    }
    student.id = max_id + 1;
    student.type = STUDENT;
    student.status = ACTIVE;
    student.enrolled_count = 0;
    
    // Write the new student to the end of the file
    lseek(fd, 0, SEEK_END);
    write(fd, &student, sizeof(Student));
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int find_student_by_id(int id, Student *student) {
    int fd;
    Student temp;
    
    // Open the file
    fd = open(STUDENT_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open student file");
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the student ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Student)) > 0) {
        if (temp.id == id) {
            *student = temp;
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

int update_student(Student student) {
    int fd;
    Student temp;
    
    // Open the file
    fd = open(STUDENT_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open student file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the student ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Student)) > 0) {
        if (temp.id == student.id) {
            // Move back to update the record
            lseek(fd, -sizeof(Student), SEEK_CUR);
            write(fd, &student, sizeof(Student));
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

int enroll_course(int student_id, int course_id) {
    Student student;
    Course course;
    
    // Find the student
    if (find_student_by_id(student_id, &student) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Find the course
    if (find_course_by_id(course_id, &course) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Check if student is already enrolled in the course
    for (int i = 0; i < student.enrolled_count; i++) {
        if (student.enrolled_courses[i] == course_id) {
            return ALREADY_EXISTS;
        }
    }
    
    // Check if the course has available seats
    if (course.available_seats <= 0) {
        return COURSE_FULL;
    }
    
    // Enroll the student
    if (student.enrolled_count < MAX_COURSES) {
        student.enrolled_courses[student.enrolled_count++] = course_id;
        
        // Update the student
        if (update_student(student) != SUCCESS) {
            return FAILURE;
        }
        
        // Update the course (decrease available seats)
        course.available_seats--;
        if (update_course(course) != SUCCESS) {
            // Roll back student enrollment
            student.enrolled_count--;
            update_student(student);
            return FAILURE;
        }
        
        return SUCCESS;
    }
    
    return FAILURE;
}

int unenroll_course(int student_id, int course_id) {
    Student student;
    Course course;
    int found = 0;
    
    // Find the student
    if (find_student_by_id(student_id, &student) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Find the course
    if (find_course_by_id(course_id, &course) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Check if student is enrolled in the course
    int i;
    for (i = 0; i < student.enrolled_count; i++) {
        if (student.enrolled_courses[i] == course_id) {
            found = 1;
            break;
        }
    }
    
    if (!found) {
        return NOT_ENROLLED;
    }
    
    // Remove the course from enrolled courses
    for (; i < student.enrolled_count - 1; i++) {
        student.enrolled_courses[i] = student.enrolled_courses[i + 1];
    }
    student.enrolled_count--;
    
    // Update the student
    if (update_student(student) != SUCCESS) {
        return FAILURE;
    }
    
    // Update the course (increase available seats)
    course.available_seats++;
    if (update_course(course) != SUCCESS) {
        // Roll back student unenrollment
        student.enrolled_courses[student.enrolled_count++] = course_id;
        update_student(student);
        return FAILURE;
    }
    
    return SUCCESS;
}

int get_enrolled_courses(int student_id, int *courses, int *count) {
    Student student;
    
    // Find the student
    if (find_student_by_id(student_id, &student) != SUCCESS) {
        return NOT_FOUND;
    }
    
    // Copy enrolled courses
    for (int i = 0; i < student.enrolled_count; i++) {
        courses[i] = student.enrolled_courses[i];
    }
    *count = student.enrolled_count;
    
    return SUCCESS;
}

int add_faculty(Faculty faculty) {
    int fd;
    Faculty temp;
    
    // Create directory if it doesn't exist
    mkdir("./data", 0777);
    
    // Open file in read-write mode, create if it doesn't exist
    fd = open(FACULTY_FILE, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open faculty file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Check if username already exists
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Faculty)) > 0) {
        if (strcmp(temp.username, faculty.username) == 0) {
            release_lock(fd, 0, 0);
            close(fd);
            return ALREADY_EXISTS;
        }
    }
    
    // Generate a new ID
    int max_id = 0;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Faculty)) > 0) {
        if (temp.id > max_id) {
            max_id = temp.id;
        }
    }
    faculty.id = max_id + 1;
    faculty.type = FACULTY;
    faculty.status = ACTIVE;
    
    // Write the new faculty to the end of the file
    lseek(fd, 0, SEEK_END);
    write(fd, &faculty, sizeof(Faculty));
    
    // Release the lock
    release_lock(fd, 0, 0);
    close(fd);
    
    return SUCCESS;
}

int find_faculty_by_id(int id, Faculty *faculty) {
    int fd;
    Faculty temp;
    
    // Open the file
    fd = open(FACULTY_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open faculty file");
        return FAILURE;
    }
    
    // Set a read lock on the file
    if (set_read_lock(fd, 0, 0) == -1) {
        perror("Failed to set read lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the faculty ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Faculty)) > 0) {
        if (temp.id == id) {
            *faculty = temp;
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

int update_faculty(Faculty faculty) {
    int fd;
    Faculty temp;
    
    // Open the file
    fd = open(FACULTY_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open faculty file");
        return FAILURE;
    }
    
    // Set a write lock on the file
    if (set_write_lock(fd, 0, 0) == -1) {
        perror("Failed to set write lock");
        close(fd);
        return FAILURE;
    }
    
    // Search for the faculty ID
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &temp, sizeof(Faculty)) > 0) {
        if (temp.id == faculty.id) {
            // Move back to update the record
            lseek(fd, -sizeof(Faculty), SEEK_CUR);
            write(fd, &faculty, sizeof(Faculty));
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