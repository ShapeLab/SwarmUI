//
//  Events.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once 

#include "ofMain.h"
#include "json.h"

namespace ofxLibwebsockets {
    
    class Connection;

    class Event {
    public:
        Event(Connection& _conn, std::string _message, bool isBinary=false);
        
        Connection& conn;
        std::string message; // message from ws OR error message if error
        Json::Value json;
        
        // binary data
        bool isBinary;
        ofBuffer data;
    };
};

/*
 class _Events {
 public:
 ofEvent<ofxLibwebsockets::Event> onConnect;
 ofEvent<ofxLibwebsockets::Event> onOpen;
 ofEvent<ofxLibwebsockets::Event> onClose;
 ofEvent<ofxLibwebsockets::Event> onIdle;
 ofEvent<ofxLibwebsockets::Event> onMessage;
 };*/