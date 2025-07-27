#include<iostream>
#include<string>
#include<cstring>

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
class Message{
    
    public:
        Message(): bodyLength_(0){}
        enum {maxBytes = 512};
        enum {header = 4};

        Message(std::string message){
            bodyLength_ = getNewBodyLength(message.size());
            encodeHeader();
            std::memcpy(data+header, message.c_str(), bodyLength_);
        }

        size_t getNewBodyLength(size_t newLength){
            if(newLength > maxBytes){
                return maxBytes;
            }
            return newLength;
        }

        std::string getData(){
            int length = header + bodyLength_;
            std::string result(data, length);
            return result;
        }

        std::string getBody(){
            std::string dataStr = getData();
            std::string result = dataStr.substr(header, bodyLength_);
            return result;
        }

        bool decodeHeader(){
            char new_header[header+1] = "";
            strncpy(new_header, data, header);
            new_header[header] = '\0';
            int headervalue = atoi(new_header);
            if(headervalue > maxBytes){
                bodyLength_ = 0;
                return false;
            }
            bodyLength_ = headervalue;
            return true;
        }

        void encodeHeader(){
            char new_header[header+1] = "";
            sprintf(new_header, "%4f", static_cast<int>(bodyLength_));
            memcpy(data, new_header, header);
        }

        size_t getBodyLength(){
            return bodyLength_;
        }

    private:
        char data[header+maxBytes];
        size_t bodyLength_;
};



#endif MESSAGE_HPP