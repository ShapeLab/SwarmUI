//
//  Connection.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once

#include "ofMain.h"
#include <libwebsockets.h>

#include <iostream>
#include <vector>
#include <string>

namespace ofxLibwebsockets {
    
    class Reactor;
    class Protocol;
    
    struct TextPacket {
        string message;
        int index;
    };
    
    struct BinaryPacket {
        unsigned char * data;
        unsigned int size;
        int index;
    };
    
    class Connection {
        friend class Reactor;
    public:
        Connection(Reactor* const _reactor=NULL, Protocol* const _protocol=NULL);
        
        ~Connection();
        
        
        void close();
        void send(const std::string& message);
        
        template <class T> 
        void sendBinary( T& image ){
            int size = image.getWidth() * image.getHeight() * image.getPixels().getNumChannels();
            sendBinary( (char *) image.getPixels().getData(), size );
        }
        
        void sendBinary( ofBuffer & buffer );
        void sendBinary( unsigned char * data, unsigned int size );
        void sendBinary( char * data, unsigned int size );
        
        // gets IP address *relative to system*
        // e.g. localhost could be ::1, 127.0.0.1, your IP, etc...
        std::string getClientIP();
        std::string getClientName();
        
        // 'private' accessor for static libwebsockets loop stuff
        
        void setupAddress();
        
        libwebsocket_context* context;
        libwebsocket* ws;
        Reactor*  reactor;
        Protocol* protocol;
        
        // operators
        bool operator==( const Connection &other );
        bool operator!=( const Connection &other );
        
        // MUST be called from main thread (e.g. client or server)
        void update();
        
        bool isIdle();
        
    protected:
        std::string client_ip;
        std::string client_name;
        
        bool binary;            // is this connection sending / receiving binary?
        
        int bufferSize;
        unsigned char* buf;
        unsigned char* binaryBuf;
        //std::vector<unsigned char> buf;
        
        // threading stuff
        std::deque<TextPacket> messages_text;
        std::deque<BinaryPacket> messages_binary;
        
        void setIdle( bool isIdle=true );
        
    private:
        bool idle;
    };
    

}
