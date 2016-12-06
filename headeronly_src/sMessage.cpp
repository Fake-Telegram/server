#include "stdafx.h"
#include "sMessage.h"
//helper.cpp----------------------
tm convertStringToTm(const string &datetime0){
	istringstream in(datetime0);
	tm datetime;
	int tmp;
	in >> tmp; datetime.tm_year = tmp - 1900; in.ignore();
	in >> tmp; datetime.tm_mon = tmp - 1; in.ignore();
	in >> datetime.tm_mday; in.ignore();
	in >> datetime.tm_hour; in.ignore();
	in >> datetime.tm_min; in.ignore();
	in >> datetime.tm_sec;
	return datetime;
}
string convertTmToString(const tm &datetime){
	ostringstream oss;
	oss << datetime.tm_year + 1900 << "/" << datetime.tm_mon + 1 << "/" << datetime.tm_mday << " "
		<< datetime.tm_hour << ":" << datetime.tm_min << ":" << datetime.tm_sec;
	return oss.str();
}
//---------------------

//конструктор для вывода из базы
sMessage::sMessage(const unsigned int &userf, const unsigned int &usert, const string &text) : m_text(text), m_from_user(userf), m_to_user(usert)
{
	time_t t = time(NULL);
	m_datetime = *localtime(&t);
	m_messageId = 0;//CreatesMessageID ???
}
sMessage::sMessage(const unsigned int &messageId, const tm &datetime, const unsigned int &userf, const unsigned int &usert, const string &text) : m_messageId(messageId), m_datetime(datetime), m_text(text), m_from_user(userf), m_to_user(usert)
{}
sMessage::sMessage(const unsigned int &messageId, const string &datetime, const unsigned int &userf, const unsigned int &usert, const string &text) : m_messageId(messageId), m_datetime(convertStringToTm(datetime)), m_text(text), m_from_user(userf), m_to_user(usert)
{}
sMessage::sMessage(const sMessage &mes)
{
	m_text = mes.m_text;
	m_datetime = mes.m_datetime;
	m_messageId = mes.m_messageId;
	m_from_user = mes.m_from_user;
	m_to_user = mes.m_to_user;
}
sMessage::~sMessage()
{
	cout << "Delete message";
}

string sMessage::get_text()
{
	return m_text;
}
unsigned int sMessage::get_id()
{
	return m_messageId;
}
unsigned int sMessage::get_userf()
{
	return m_from_user;
}
unsigned int sMessage::get_usert()
{
	return m_to_user;
}

tm sMessage::get_datetime()
{
	return m_datetime;
}
string sMessage::get_string_datetime()
{	
	return  convertTmToString(m_datetime);
}
bool sMessage::getMessage(ostream &s)
{
	s << m_messageId << " " << m_from_user << " " << m_to_user << " " << convertTmToString(m_datetime) << " " << m_text << endl;
	//return false;

	return true;
}
