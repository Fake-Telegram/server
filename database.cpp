#include "database.hpp"

Data::Data(std::string pw, unsigned new_id) {
    password = pw;
    id = new_id;
};

std::string Database::authorization(
    rapidjson::Document &doc,
    const boost::asio::ip::tcp::socket& socket
) 
{
    rapidjson::StringBuffer result;
    rapidjson::Writer<rapidjson::StringBuffer> writer(result);
    std::map<std::string, Data>::iterator it = 
        auth_pair.find(doc["login"].GetString());
    
    writer.StartObject();
    writer.Key("operation");
    writer.Int(AUTHORIZATION);
    if (it == auth_pair.end()) {
        writer.Key("result");
        writer.Bool(false);
        writer.EndObject();
        return result.GetString();
    } 
    if (it->second.password != doc["password"].GetString()) {
        writer.Key("result");
        writer.Bool(false);
        writer.EndObject();
        return result.GetString();
    }
    connection.insert(
        std::pair<unsigned, const boost::asio::ip::tcp::socket&>
        (it->second.id, socket)
    );
    writer.Key("result");
    writer.Bool(true);
    writer.Key("id");
    writer.Uint(it->second.id);
    writer.EndObject();
    return result.GetString();
}

std::string Database::registration(
    rapidjson::Document &doc,
    const boost::asio::ip::tcp::socket& socket
)
{
    rapidjson::StringBuffer result;
    rapidjson::Writer<rapidjson::StringBuffer> writer(result);
    std::map<std::string, Data>::iterator it = 
        auth_pair.find(doc["login"].GetString());

    writer.StartObject();
    writer.Key("operation");
    writer.Int(REGISTRATION);
    if (it != auth_pair.end()) {
        writer.Key("result");
        writer.Bool(false);
        writer.EndObject();
        return result.GetString();
    }
    Data data(doc["password"].GetString(), id++);
    auth_pair.insert(
        std::pair<std::string, Data>
        (doc["login"].GetString(), data)
    ); 
    connection.insert(
        std::pair<unsigned, const boost::asio::ip::tcp::socket&>
        (it->second.id, socket)
    );
    writer.Key("result");
    writer.Bool(true);
    writer.Key("id");
    writer.Uint(id - 1);
    writer.EndObject();
    return result.GetString();
}

Database::Database()
{
   Data data("admin", id++);
   auth_pair.insert(
      std::pair<std::string, Data>
    ("admin", data)
   );
};

