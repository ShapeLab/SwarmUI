//
//  Message.hpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 28/01/16.
//
//

#ifndef Message_hpp
#define Message_hpp

#include "ofMain.h"
#include <iostream>
#include <stdio.h>


class Message
{
private:
    uint8_t type;
    uint8_t senderId;
    uint8_t* payload;
    
public:
    Message();
    Message(uint8_t _senderId, uint8_t _type, uint8_t* _payload);
    ~Message();

    void setType(uint8_t _type);
    uint8_t getType();
    
    void setSenderId(uint8_t _senderId);
    uint8_t getSenderId();
    
    void setPayload(uint8_t* _payload);
    uint8_t* getPayload();
    
    uint8_t* ToByteArray();
    
    string IntoString();
    string ToHexString();
};



#endif /* Message_hpp */
