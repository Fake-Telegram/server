#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "settings.hpp"
#include "database.hpp"

#define BUF_SIZE 2048

boost::mutex global_stream_lock;
unsigned Database::id = 1;


class Tcp_connection
    : public boost::enable_shared_from_this<Tcp_connection>
{
public:
    typedef boost::shared_ptr<Tcp_connection> pointer;
    char recv_buffer[BUF_SIZE];
    std::string receive_string;
    size_t recv_buffer_index;
    std::string send_buffer;
    boost::asio::ip::tcp::socket socket;

    static pointer create(
        boost::asio::io_service& io_service,
        Database &db
    ); 
    void send_message(std::string &buffer);
    void get_message();
    void start(); 
    ~Tcp_connection();

private:
    Database &db;

    Tcp_connection(
        boost::asio::io_service& io_service,
        Database &db
    );
    void handle_get(
        const boost::system::error_code &ec,
        size_t num_got);
    void handle_send(
        const boost::system::error_code &ec,
        std::string &buffer,
        std::size_t num_sent);
    std::string handle_input(void);
};



Tcp_connection::pointer Tcp_connection::create(
    boost::asio::io_service& io_service,
    Database &db
) 
{
    return pointer(new Tcp_connection(io_service, db));
}


void Tcp_connection::start() {
    send_buffer= std::string("Hello!");

    get_message();
    send_message(send_buffer);
    std::cerr << receive_string << std::endl;
}

void Tcp_connection::send_message(std::string &buffer)
{
    socket.async_write_some(
        boost::asio::buffer(buffer),
        boost::bind(
            &Tcp_connection::handle_send,
            shared_from_this(),
            boost::asio::placeholders::error,
            buffer,
            boost::asio::placeholders::bytes_transferred()
        )
    );  
}

void Tcp_connection::get_message()
{
    socket.async_read_some(
        boost::asio::buffer(recv_buffer, BUF_SIZE),
        boost::bind(
            &Tcp_connection::handle_get, 
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred()
        )
    );
}

Tcp_connection::Tcp_connection(
    boost::asio::io_service& io_service,
    Database &new_db
)
    : socket(io_service)
    , db(new_db)
{
}

Tcp_connection::~Tcp_connection()
{
    boost::system::error_code ec;
    socket.shutdown(
        boost::asio::ip::tcp::socket::shutdown_both, ec
    );
    socket.close(ec);
}
       
std::string Tcp_connection::handle_input(void)
{
    unsigned ID_operation;
    rapidjson::Document doc;
    doc.SetObject();
    if(!doc.Parse(receive_string.c_str()).HasParseError()){
        ID_operation = doc["operation"].GetInt();
        switch (ID_operation) {
        case AUTHORIZATION:
            return db.authorization(doc, socket);
        default:
            break;
        }
    } else {
        std::cerr << receive_string << std::endl;
    }
    return "Not handled";
}

void Tcp_connection::handle_get(
    const boost::system::error_code &ec,
    size_t num_got)
{
    if (ec) {
        global_stream_lock.lock();
        std::cerr << "[" << 
            "] Error: " << ec <<std::endl;
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cerr << "[" << 
            "] Receive " << num_got << 
            "bytes." << std::endl;
        global_stream_lock.unlock();
        receive_string = /*receive_string + */
            std::string(recv_buffer, num_got);
        send_buffer = handle_input();
        send_message(send_buffer);
        get_message();
    }
}

void Tcp_connection::handle_send(
    const boost::system::error_code &ec,
    std::string &buffer,
    std::size_t num_sent)
{
    if (ec) {
        global_stream_lock.lock();
        std::cerr << "[" << 
            "] Error: " << ec << std::endl;
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cerr << "[" <<
            "] Sent " << num_sent << "bytes." <<std::endl;
        global_stream_lock.unlock();
        //std::cerr << "Result: " << buffer <<  "#" << std::endl;   
        buffer.erase(0, num_sent);
        if (!buffer.empty()) {
            socket.async_write_some(
                boost::asio::buffer(buffer),
                boost::bind(
                    &Tcp_connection:: handle_send,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    buffer,
                    boost::asio::placeholders::bytes_transferred()
                )
            );
        }
    }
}


class Tcp_server
{
public:
    Database db;

private:
    boost::asio::ip::tcp::acceptor acceptor_;

public:
    Tcp_server(boost::asio::io_service& io_service)
        : acceptor_(
            io_service,
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), 
                7000
            )
        )
    {
        start_accept();
    }

private:
    void start_accept()
    {
        Tcp_connection::pointer new_connection = 
            Tcp_connection::create(
                acceptor_.get_io_service(),
                db
            );

        acceptor_.async_accept(new_connection->socket,
            boost::bind(
                &Tcp_server::handle_accept, 
                this,
                new_connection,
                boost::asio::placeholders::error
            )
        );
    }

    void handle_accept(
        Tcp_connection::pointer new_connection,
        const boost::system::error_code& error)
    {
        if (error) {
            global_stream_lock.lock();
            std::cerr << "[" << boost::this_thread::get_id() <<
                "] Error: " << error << std::endl;
            global_stream_lock.unlock();
        } else {
            global_stream_lock.lock();
            std::cerr << "[" << boost::this_thread::get_id() <<
                "] Accepted!" << std::endl;
            global_stream_lock.unlock();
            new_connection->start();
            start_accept();
        }
    }
};


int main()
{
    try {
        boost::asio::io_service io_service;
        boost::shared_ptr<boost::asio::io_service::work> work (
                new boost::asio::io_service:: work(io_service)
        );
        Tcp_server server(io_service);
        io_service.run();
    } 
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

/*

void 
thread_handler(boost::shared_ptr<boost::asio::io_service> io_service)
{

    global_stream_lock.lock();
    std::cerr << "[" << boost::this_thread::get_id() << 
        "] Thread Start\n";
    global_stream_lock.unlock();
    try {
        boost::system::error_code ec;
        
        io_service->run(ec);
        if (ec) {
            global_stream_lock.lock();
            std::cerr << "[" << boost::this_thread::get_id() <<
                "] Error: " << ec  << std::endl;
            global_stream_lock.unlock();
        }
        break;
    } catch (std::exception &ex) {
        global_stream_lock.lock();
        std::cerr << "[" << boost::this_thread::get_id() <<
            "] Exception: " << ex.what() << std::endl;
        global_stream_lock.unlock();
    }
}

struct Client: 
public boost::enable_shared_from_this<ClientContext>
{
    boost::asio::ip::tcp::socket socket;

    char recv_buffer[BUF_SIZE];
    std::string receive_string;
    size_t recv_buffer_index;

    std::string send_buffer;

    }

    void get_handler(
        const boost::system::error_code &ec,
        size_t num_got)
    {
        if (ec) {
            global_stream_lock.lock();
            std::cerr << "[" << boost::this_thread::get_id() <<
                "] Error: " << ec <<std::endl;
            global_stream_lock.unlock();
            Close();
        } else {
            global_stream_lock.lock();
            std::cerr << "[" << boost::this_thread::get_id() <<
                "] Receive " << num_got << 
                "bytes." << std::endl;
            global_stream_lock.unlock();
            receive_string = reseive_string + 
                std::string(recv_buffer, num_got);
            get_message();
        }
    }

    void get_message();
    {
        m_socket.async_read_some(
            boost::asio::buffer(recv_buffer, BUF_SIZE),
            boost::bind(
                &Client::get_handler, 
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred()
            )
        );
    }
};

void
accept_handler(
    const boost::system::error_code &ec,
    boost::shared_ptr<Client> client
)
{
    if (ec) {
        global_stream_lock.lock();
        std::cerr << "[" << boost::this_thread::get_id() <<
            "] Error: " << ec << std::endl;
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cerr << "[" << boost::this_thread::get_id() <<
            "] Accepted!" << std::endl;
        global_stream_lock.unlock();
        //client->send_message("Hi", 6);
        //client->get_message();
    }
}


int 
main( int argc, char *argv[])
{
    boost::shared_ptr<boost::asio::io_service> io_service(
        new boost::asio::io_service
    );
    boost::shared_ptr<boost::asio::io_service::work> work (
            new boost::asio::io_service:: work(*io_service)
    );
    boost::asio::ip::tcp::socket listening_socket(*io_service);
    boost::thread_group worker_threads;

    for (int i = 0; i < 1; i++) {
        worker_threads.create_thread(boost::bind(&WorkerThread,
                io_service));
    }
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor(
        new boost::asio::ip::tcp::acceptor(*io_service)
    );
    boost::shared_ptr<Client> client(
        new Client(listening_socket)
        );
    try {
        boost::asio::ip::tcp::resolver resolver(*io_service);
        boost::asio::ip::tcp::resolver::query query(
            "127.0.0.1",
            boost::lexical_cast<std::string>(7777)
        );
        boost::asio::ip::tcp::endpoint endpoint = 
            *resolver.resolve(query);
        acceptor->open(endpoint.protocol());
        acceptor->set_option(
            boost::asio::ip::tcp::acceptor::reuse_address(false)
        );
        acceptor->bind(endpoint);
        acceptor->listen(boost::asio::socket_base::max_connections);
        acceptor->async_accept(client->socket, boost::bind(
            OnAccept, _1, client
            )
        );
        
        global_stream_lock.lock();
        std::cout << "Listening on: " << endpoint << std::endl;
        global_stream_lock.unlock();
    } catch(std:: exception &ex) {
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id() <<
            "] Exception: " << ex.what() << std::endl;
        global_stream_lock.unlock();
    }
    std::cin.get();
    boost::system::error_code ec;
    acceptor->close(ec);
    io_service->stop();
    worker_threads.join_all();

    return 0;
}*/
