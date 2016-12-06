//#pragma once
#ifndef SUSER_H
#define SUSER_H

#include <iostream>
#include <string>

using namespace std;
class sUser
{
protected:
	unsigned int m_userId;
	string m_login;
	string m_password;
	string m_name;
public:
	sUser(const unsigned int &userId, const string &login, const string &password, const string &name);
	sUser(const sUser &user);
	~sUser();
	unsigned int get_id();
	string get_login();
	string get_name();
	
	void set_userId(const unsigned int userId);
	void change_name(const string &newName);

	bool operator==(const sUser& right);
};
#endif

