#include <map>
#include <string>
#include <boost/asio.hpp>
#include "settings.hpp"

class Data {
public:
    std::string password;
    unsigned id;

    Data(std::string pw, unsigned new_id);
};

class Database {
public:
    static unsigned id;
    std::map <
        unsigned, 
        const boost::asio::ip::tcp::socket&
    > connection;
    std::map <std::string, Data> auth_pair;

    std::string authorization(
        rapidjson::Document &doc,
        const boost::asio::ip::tcp::socket&
    );
    std::string registration(
        rapidjson::Document &doc,
        const boost::asio::ip::tcp::socket&
   );
   Database();
};

