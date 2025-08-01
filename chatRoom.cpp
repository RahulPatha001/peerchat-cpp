#include "chatRoom.hpp"

void Room::join(participantPtr participant){
    this->participants.insert(participant);
}

void Room::leave(participantPtr participant){
    this->participants.erase(participant);
}

void Room::deliver(participantPtr participant, Message &message){
    messageQueue.push_back(message);
    while(!messageQueue.empty()){
        Message msg = messageQueue.front();
        messageQueue.pop_front();

        for(participantPtr _participant: participants){
            if(participant != _participant){
                _participant->write(msg);
            }
        }
    }
}

void Session::async_read(){
    auto self(shared_from_this());
    boost::asio::async_read_until(clientSocket, buffer, "\n", [this, self](boost::system::error_code ec, std::size_t bytes_transferred){
        if(!ec){
            std::string data(boost::asio::buffers_begin(buffer.data()),
        boost::asio::buffers_begin(buffer.data())+bytes_transferred);

        buffer.consume(bytes_transferred);
        std::cout<<"Recived:" <<data<<std::endl;
        Message message(data);

        deliver(message);

        async_read();

        }else{
            room.leave(shared_from_this());
            if(ec == boost::asio::error::eof){
                std::cout<<"Connection closed by peer"<<std::endl;
            }else{
                std::cout<<"Read error: "<<ec.message()<<std::endl;
            }
        }
    });
}


void Session::async_write(std::string messageBody, size_t messageLength){
    auto write_handler = [&](boost::system::error_code ec, std::size_t bytes_transferred){
        if(!ec){
            std::cout<<"Data is written to the socket"<<std::endl;
        }else{
            std::cerr<<"Write error: "<<ec.message()<<std::endl;
        }
    };
    boost::asio::async_write(clientSocket, boost::asio::buffer(messageBody, messageLength), write_handler);
}

void Session::write(Message &message){
    messageQueue.push_back(message);
    while (messageQueue.size() != 0)
    {
        Message message = messageQueue.front();
        messageQueue.pop_front();
        bool header_decode = message.decodeHeader();
        if(header_decode){
            std::string body = message.getBody();
            async_write(body, message.getBodyLength());
        }else{
            std::cout << "message length exeeds max length"<<std::endl;
        }
    }
    
}


void Session::deliver(Message &message){
    room.deliver(shared_from_this(), message);
}

void Session::start(){
    room.join(shared_from_this());
    async_read();
}

using boost::asio::ip::address_v4;

void accept_connection(boost::asio::io_context &io, char *port, tcp::acceptor &acceptor,
            Room &room, const tcp::endpoint &endpoint){
                tcp::socket socket(io);
                acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket){
                    if(!ec){
                        std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket), room);
                        
                        session->start();

                    }
                    accept_connection(io, port, acceptor, room ,endpoint);
                });
}

int main(int argc, char*argv[]){
    try
    {
        if(argc<2){
            std::cerr<<"usage server <port>";
            return 1;
        }
        Room room;
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
        tcp::acceptor acceptor(io_context, endpoint);
        accept_connection(io_context, argv[1], acceptor, room, endpoint);
        io_context.run();
    }
    catch(const std::exception& e)
    {
        std::cerr <<"Exception occured " << e.what() << '\n';
    }

    return 0;
    
}