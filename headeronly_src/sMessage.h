#ifndef SMESSAGE_H
#define SMESSAGE_H

#include <iostream>
#include <ctime>
#include <string>
using namespace std;

class sMessage{
private:
	unsigned int m_messageId;
	string m_text;
	unsigned int m_from_user;
	unsigned int m_to_user;
	tm m_datetime;
public:
	sMessage(const unsigned int &userf, const unsigned int &usert, const string &text);
	sMessage(const unsigned int &messageId, const tm &datetime, const unsigned int &userf, const unsigned int &usert, const string &text);
	sMessage(const unsigned int &messageId, const string &datetime, const unsigned int &userf, const unsigned int &usert, const string &text);
	sMessage(const sMessage &other);
	string get_text();
	unsigned int get_id();
	unsigned int get_userf();
	unsigned int get_usert();
	tm get_datetime();
	string get_string_datetime();
	bool getMessage(ostream &);
	~sMessage();
};

#endif // SMESSAGE_H
