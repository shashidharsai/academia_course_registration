#ifndef AUTH_H
#define AUTH_H

#include "user.h"

// Authenticate a user
int login(char *username, char *password, User *user);

// Check if user is active
int is_active(User user);

#endif