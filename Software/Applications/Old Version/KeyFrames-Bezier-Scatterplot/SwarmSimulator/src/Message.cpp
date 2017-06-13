//
//  Message.cpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 28/01/16.
//
//

#include "Message.hpp"

Message::Message()
{
    senderId = 0;
    type = 0;
    payload = nullptr;
}

Message::~Message()
{
}

Message::Message(uint8_t _senderId, uint8_t _type, uint8_t* _payload)
{
    senderId = _senderId;
    type = _type;
    payload = _payload;
}

void Message::setType(uint8_t _type)
{
    type = _type;
}

uint8_t Message::getType()
{
    return type;
}

void Message::setSenderId(uint8_t _senderId)
{
    senderId = _senderId;
}

uint8_t Message::getSenderId()
{
    return senderId;
}

void Message::setPayload(uint8_t* _payload)
{
    payload = _payload;
}

uint8_t* Message::getPayload()
{
    return payload;
}

uint8_t* Message::ToByteArray()
{
    uint8_t* array = new uint8_t[sizeof(payload)+2];
    array[0] = type;
    array[1] = senderId;
    for(int i=0;i<sizeof(payload);i++)
        array[i+2]=payload[i];
    return array;
}

string Message::IntoString()
{
    if(payload != nullptr)
    {
        string s = string();
        
        if(senderId!='\n')
            s += (char)senderId;
        else
            s += "\\n";
        
        if(type!='\n')
            s += (char)type;
        else
            s += "\\n";
        
        for (int i = 0; i < sizeof(payload); i++)
        {
            if (payload[i] == '\n')
                s += "\\n";
            else
                s += (char)payload[i];
        }
        
        return s;
    }
    else
        return nullptr;
}

string Message::ToHexString()
{
    std::stringstream stream;
    stream << "0x" << std::setfill ('0') << std::setw(2) << std::hex << senderId;
    stream << " 0x" << std::setfill ('0') << std::setw(2) << std::hex << type;
    
    
    for (int i = 0; i < sizeof(payload); i++)
    {
        if (payload[i] == '\n')
            stream << endl;
        else
            stream << " 0x" << std::setfill ('0') << std::setw(2) << std::hex << payload[i];
    }
    return stream.str();
}

