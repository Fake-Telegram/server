#include "database.hpp"
#include "headeronly_src/bd.h"

Data::Data(std::string pw, unsigned new_id) {
    password = pw;
    id = new_id;
};

std::string convertTmToString(const tm &datetime){
std::ostringstream oss;
oss << 
    datetime.tm_year + 1900 <<
    "/" << 
    datetime.tm_mon + 1 << 
    "/" << 
    datetime.tm_mday <<
    " " << 
    datetime.tm_hour << 
    ":" << 
    datetime.tm_min << 
    ":" << 
    datetime.tm_sec
;
return oss.str();
}


std::string Database::user_message(
    rapidjson::Document &doc,
    const boost::asio::ip::tcp::socket& socket
) 
{
    Message mes;
    rapidjson::StringBuffer result;
    rapidjson::Writer<rapidjson::StringBuffer> writer(result);
    rapidjson::StringBuffer answer;
    rapidjson::Writer<rapidjson::StringBuffer> parlamenter(answer);
    std::map<unsigned, std::list<unsigned>>::iterator it = 
        chats.find(doc["chatId"].GetUint());
    std::map<unsigned, std::list<Message>>::iterator m_it;
    writer.StartObject();
    writer.Key("operation");
    writer.Int(SENT);
    writer.Key("chatId");
    writer.Uint(doc["chatId"]. GetUint());
    writer.Key("localId");
    writer.Uint(doc["localId"].GetUint());
    time_t t = time(NULL);
    tm datetime = *localtime(&t);
    writer.Key("datetime");
    writer.String(convertTmToString(datetime).c_str()); 
    parlamenter.StartObject();
    parlamenter.Key("operation");
    parlamenter.Int(MESSAGE);
    parlamenter.Key("chatId");
    parlamenter.Uint(doc["chatId"].GetUint());
    parlamenter.Key("datetime");
    parlamenter.String(convertTmToString(datetime).c_str());
    if (it == chats.end()) {
        writer.Key("result");
        writer.Bool(false);
        writer.EndObject();
        return result.GetString();
    } 
    m_it = messages.find(doc["chatId"].GetUint());
    if  (!m_it->second.empty()) {
        mes.id = m_it->second.back().id + 1;
    } else {
        mes.id = 0;
    }
    parlamenter.Key("id");
    parlamenter.Uint(mes.id);//generate
    parlamenter.Key("text");
    parlamenter.String(doc["text"].GetString());
    parlamenter.EndObject();
    mes.text = answer.GetString();
    m_it->second.push_back(mes);
    writer.Key("result");
    writer.Bool(true);
    writer.Key("id");
    writer.Uint(mes.id);
    writer.EndObject();
    return answer.GetString();
}

std::string Database::authorization(
    rapidjson::Document &doc,
    const boost::asio::ip::tcp::socket& socket,
    unsigned& user_id
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
    user_id = it->second.id;
    printf("User_id is %d.\n", user_id);
    writer.Key("result");
    writer.Bool(true);
    writer.Key("id");
    writer.Uint(it->second.id);
    writer.EndObject();
    return result.GetString();
}

std::string Database::user_registration(
    rapidjson::Document &doc,
    const boost::asio::ip::tcp::socket& socket,
    unsigned& user_id
)
{
    //bool res;
    rapidjson::StringBuffer result;
    rapidjson::Writer<rapidjson::StringBuffer> writer(result);
    std::map<std::string, Data>::iterator it = 
        auth_pair.find(doc["login"].GetString());

    writer.StartObject();
    writer.Key("operation");
    writer.Int(REGISTRATION);
/*    res = registration(
        doc["login"].GetString(), 
        doc["password"].GetString()
    );
    if (res) {
        writer.Key("result");
        writer.Bool(true);
    } else {
        writer.Key("result");
        writer.Bool(true);
    }*/
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
    user_id = it->second.id;
    //writer.Key("id");
    //writer.Uint(id - 1);
    writer.EndObject();
    return result.GetString();
}

Database::Database()
{
   Data data("admin", id++);
   Data data_0("123", id++);
   State st;
   std::list<State> l_s;
   auth_pair.insert(
      std::pair<std::string, Data>
      ("admin", data)
   );
   auth_pair.insert(
      std::pair<std::string, Data>
      ("123", data_0)
   );
   std::list<unsigned> l_u;
   l_u.push_back(1);
   l_u.push_back(2);
   chats.insert(
      std::pair<unsigned, std::list<unsigned>>
      (1, l_u)
   );
   st.chat_id = 1;
   st.current_mes = 0;
   l_s.push_back(st);
   states.insert(
      std::pair<unsigned, std::list<State>>
      (1, l_s)
   );
   std::cerr << states[1].begin()->chat_id << std::endl;
   states.insert(
      std::pair<unsigned, std::list<State>>
      (2, l_s)
   );
   std::cerr << states[2].begin()->chat_id << std::endl;
};

