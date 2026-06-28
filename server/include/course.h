#ifndef COURSE_H
#define COURSE_H

#include "constants.h"
#include "student_handler.h"
#include "user.h"

struct User;

typedef struct {
    int id;
    char course_id[MAX_COURSE_ID];
    char name[MAX_COURSE_NAME];
    int faculty_id;
    int max_seats;
    int available_seats;
    int status; // 1 for active, 0 for inactive
} Course;

typedef struct {
    int course_id;
    int student_id;
} Enrollment;

// Course functions
int add_course(Course course);
int find_course_by_id(int id, Course *course);
int update_course(Course course);
int remove_course(int id);
int get_all_courses(Course *courses, int *count);
int get_available_courses(Course *courses, int *count);
int get_course_enrollments(int course_id, int *student_ids, int *count);

// Enrollment functions
int enroll_student(int course_id, int student_id);
int unenroll_student(int course_id, int student_id);
int is_enrolled(int course_id, int student_id);

#endif