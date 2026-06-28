#ifndef USER_H
#define USER_H

#include "constants.h"

struct Course;

typedef struct {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    int type;
    int status; // 1 for active, 0 for inactive
} User;

// For Student specific details
typedef struct {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    int type;
    int status; // 1 for active, 0 for inactive
    int enrolled_courses[MAX_COURSES];
    int enrolled_count;
} Student;

// For Faculty specific details
typedef struct {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    int type;
    int status; // 1 for active, 0 for inactive
    char department[MAX_NAME];
} Faculty;

// User functions
int add_user(User user);
int find_user_by_username(char *username, User *user);
int find_user_by_id(int id, User *user, int type);
int authenticate_user(char *username, char *password, User *user);
int update_user(User user);
int change_password(int id, char *new_password, int type);
int activate_deactivate_user(int id, int status, int type);

// Student specific functions
int add_student(Student student);
int find_student_by_id(int id, Student *student);
int update_student(Student student);
int enroll_course(int student_id, int course_id);
int unenroll_course(int student_id, int course_id);
int get_enrolled_courses(int student_id, int *courses, int *count);

// Faculty specific functions
int add_faculty(Faculty faculty);
int find_faculty_by_id(int id, Faculty *faculty);
int update_faculty(Faculty faculty);

#endif