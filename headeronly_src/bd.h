#ifndef BD_H
#define BD_H

#include <string>
#include <iostream>
#include "sqlite3pp.h"
#include "sqlite3ppext.h"
#include <string.h>
#include <cstring>
#include "sUser.h"

using namespace std;

int create_message(const char* message, int id_user_from, int id_chat);
bool registration(const char* email, const char* password);
bool email_exists(const char* email);//true = exists
int authorization (const char* email, const char* password);//0 if not auth.
sUser* get_user_by_id (const int id);

#endif
