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
            
        }
    }
    
}
