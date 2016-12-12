#include <string>
#include <iostream>
#include "sqlite3pp.h"
#include "sqlite3ppext.h"
#include <string.h>
#include <cstring>
#include "bd.h"
#include "sUser.h"

int main (void){

	sqlite3pp::database db("./../test.db");
	{
        //создаем таблицу пользователей
        db.execute("CREATE TABLE IF NOT EXISTS users ( id varint primary key, email varchar(65), pass varchar(65), nickname varchar(65))");
        //таблица чатов
        db.execute("CREATE TABLE IF NOT EXISTS chats ( id varint primary key, id_user_1 varint, id_user_2 varint)");
        //таблица чат-пользователь
        db.execute("CREATE TABLE IF NOT EXISTS chat_user ( id varint primary key, id_user, id_chat)");
        //таблица сообщений
        db.execute("CREATE TABLE IF NOT EXISTS messages ( id varint primary key, message varchar(500), id_user_from varint, id_chat varint, datetime varchar(20))");
        //таблица пользователь-пользователь (на случай возможности дружить)
        db.execute("CREATE TABLE IF NOT EXISTS user_user ( id varint primary key, id_user_1, id_user_2)");
        //создадим несколько пользователей
        db.execute("INSERT INTO users (id, email, pass) VALUES (1, 'Vanya', '1234dsvds')");
        db.execute("INSERT INTO users (id, email, pass) VALUES (2, 'KAtya', '1dsfs2dvbdf34')");
        db.execute("INSERT INTO chats (id) VALUES (1)");
        db.execute("INSERT INTO chats (id) VALUES (2)");


        db.execute("INSERT INTO chat_user (id, id_user, id_chat) VALUES (1, 2, 1)"); //пользователь с id=2 состоит в чате с id=1
        int i = 3;
        sqlite3pp::command cmd(db, "INSERT INTO users (id, email, pass) VALUES (?, ?, ?)");
        cmd.binder() << i << "Mike" << "555sff4";
        cmd.execute();

        //создадим сообщения
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (1, 'privet', 1, 1)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (2, 'privet1', 2, 2)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (3, 'privet2', 3, 1)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (4, 'privet3', 1, 1)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (5, 'privet4', 3, 2)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (6, 'privet5', 2, 1)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (7, 'privet6', 1, 2)");
        db.execute("INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (8, 'privet7', 3, 1)");

        create_message("Тестовое сообщение", 1, 2);

        //выведем все сообщения от пользователя с id=1
        sqlite3pp::query qry(db, "SELECT id, message, id_user_from, id_chat FROM messages");
        for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
            if ((*i).get<int>(2) ==1){
                for (int j = 0; j < qry.column_count(); ++j) {
                    cout << (*i).get<char const*>(j) << "\t";
                }
                cout << endl;
            }
            
        }

    }

    sqlite3pp::query qry(db, "SELECT id, email, pass FROM users");

      // for (int i = 0; i < qry.column_count(); ++i) {
    		// cout << qry.column_name(i) << "\t";
    		// cout << endl;
      // }
    
    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
    	//cout << (*i).id;
    	for (int j = 0; j < qry.column_count(); ++j) {
      		cout << (*i).get<char const*>(j) << "\t";
    	}
    	cout << endl;
    }
}



int create_message(const char* message, int id_user_from, int id_chat){
    int j = 9;
    sqlite3pp::database db("test.db");
    sqlite3pp::command cmd(db, "INSERT INTO messages (id, message, id_user_from, id_chat) VALUES (?, ?, ?, ?)");
    cmd.binder() << j << message << id_user_from << id_chat;
    cmd.execute();
    j++;
    return 0;
}

bool email_exists(const char* email){
    bool answer = false;
    sqlite3pp::database db("test.db");
    sqlite3pp::query qry(db, "SELECT email FROM users");
    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
            if (strcmp((*i).get<char const*>(0), email)==0){
                answer = true;
                break;
            }
    }
    return answer;
}

bool registration(const char* email, const char* password){
    if (email_exists(email)) return false;
    else{
        sqlite3pp::database db("test.db");
        sqlite3pp::command cmd(db, "INSERT INTO users (id, email, pass) VALUES (?, ?, ?)");
        cmd.binder() << 13 << email << password;
        cmd.execute();
        return true;
    }
}

int authorization (const char* email, const char* password){
    sqlite3pp::database db("test.db");
    sqlite3pp::query qry(db, "SELECT email, pass, id FROM users");
    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
            if (strcmp((*i).get<char const*>(0), email)==0){
                if(strcmp((*i).get<char const*>(1), password)==0)
                    return (*i).get<int>(2);
                else return 0;
            }
    }

}

/*sUser* get_user_by_id (const int id){
	sqlite3pp::database db("test.db");
    sqlite3pp::query qry(db, "SELECT email, pass, id FROM users");
    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
    	if ((*i).get<int>(2)==id){
                
            sUser user ((*i).get<int>(2), (*i).get<char const*>(0), (*i).get<char const*>(1), (*i).get<char const*>(1));
			return &user;

        }

    }

}
*/
//авторизация / проверка логина пороля / регистрация
//достать все сообщения чата
//выводить list сообщений
//
