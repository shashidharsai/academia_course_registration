#include "../include/auth.h"
#include <string.h>

int login(char *username, char *password, User *user) {
    // Find user by username
    if (find_user_by_username(username, user) != SUCCESS) {
        return FAILURE;
    }
    
    // Check if password matches
    if (strcmp(user->password, password) != 0) {
        return FAILURE;
    }
    
    // Check if user is active
    if (user->status != ACTIVE) {
        return INACTIVE;
    }
    
    return SUCCESS;
}

int is_active(User user) {
    return user.status == ACTIVE;
}