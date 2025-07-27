#include<iostream>
#include"message.hpp"
#include<set>
#include<memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<unistd.h>
#include<boost/asio.hpp>
#include<deque>

#ifndef CHATROOM_HPP
#define CHATROOM_HPP

using boost::asio::ip::tcp;

    class participant{

        public:
            virtual void deliver(Message &message) = 0;
            virtual void write(Message &message) = 0;
            virtual ~participant() = default;
    };

    typedef std::shared_ptr<participant> participantPtr;

    class Room{
        public:
            void join(participantPtr participant);
            void leave(participantPtr pariticipant);
            void deliver(participantPtr participant, Message &message);
        private:
            std::deque<Message> messageQueue;
            enum {MaxParticipats = 100};
            std::set<participantPtr> participants;
    };

    class Session: public participant, public std:: enable_shared_from_this<Session>{
        public:
            Session(tcp::socket s, Room &room);
            void start();
            void deliver(Message &message);
            void write(Message &message);
            void async_write(std::string messageBody, size_t messageLength);
        private:
            tcp::socket clientSocket;
            boost::asio::streambuf buffer;
            Room &room;
            std::deque<Message> messageQueue;
    };
#endif CHATROOM_HPP