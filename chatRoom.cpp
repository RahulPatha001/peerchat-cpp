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
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}