#include <map>
#include <string>
#include <boost/asio.hpp>
#include <list>
#include <ctime>
#include <sstream>
#include "settings.hpp"

class Data {
public:
    std::string password;
    unsigned id;

    Data(std::string pw, unsigned new_id);
};

class Message {
public:
    std::string text;
    unsigned id;
};

class State {
public:
    unsigned chat_id;
    unsigned current_mes;
};

class Database {
public:
    static unsigned id;

    std::map <
        unsigned,
        std::list<State>
    > states;

    std::map <
        unsigned, 
        const boost::asio::ip::tcp::socket&
    > connection;

    std::map <std::string, Data> auth_pair;

    std::map <
        unsigned,
        std::list<unsigned>
    > chats;
    
    std::map <
        unsigned,
        std::list<Message>
    > messages;

    std::string authorization(
        rapidjson::Document &doc,
        const boost::asio::ip::tcp::socket&,
        unsigned& user_id
    );
    std::string user_registration(
        rapidjson::Document &doc,
        const boost::asio::ip::tcp::socket&,
        unsigned& user_id
    );
    std::string user_message(
        rapidjson::Document &doc,
        const boost::asio::ip::tcp::socket& socket
    );
    Database();
};

