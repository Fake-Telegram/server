#include "sUser.h"


sUser::sUser(const unsigned int &userId, const string &login, const string &password, const string &name) :  m_userId(userId), m_login(login), m_password(password), m_name(name)
{}
sUser::sUser(const sUser &user)
{
	m_userId = user.m_userId;
	m_login = user.m_login;
	m_password = user.m_password;
	m_name = user.m_name;
}
sUser::~sUser()
{
	//cout << "Delete User";
}
unsigned int sUser::get_id()
{
	return m_userId;
}
string sUser::get_login()
{
	return m_login;
}
string sUser::get_name()
{
	return m_name;
}


void sUser::set_userId(const unsigned int userId)
{
	m_userId = userId;
}

void sUser::change_name(const string &newName)
{
	m_name = newName;
}

bool sUser::operator==(const sUser& right)
{
	return m_userId == right.m_userId;
}
