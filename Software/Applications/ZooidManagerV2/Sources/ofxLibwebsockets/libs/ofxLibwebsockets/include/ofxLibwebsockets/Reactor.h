//
//  Reactor.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once

#include "ofThread.h"
#include "ofEvents.h"
#include <libwebsockets.h>
#include "ofxLibwebsockets/Protocol.h"
#include "ofxLibwebsockets/Connection.h"

namespace ofxLibwebsockets {
        
    class Reactor : public ofThread {
        friend class Protocol;
        
    public:
        Reactor();
        ~Reactor();
        void exit();
        
        // methods
        virtual void close() = 0; // close main ws connection (or close server)
        void close(Connection* const conn); // close a specific connection
        void registerProtocol(const std::string& name, Protocol& protocol);
        
        // parse JSON automatically? (true by default)
        bool bParseJSON;
        
        // getters
        struct libwebsocket_context *   getContext();
        vector<Connection *>            getConnections();
        Connection *                    getConnection( int index );
        
        Protocol* const protocol(const unsigned int idx);
        std::vector<std::pair<std::string, Protocol*> > protocols;
        
        //private:
        unsigned int _allow(struct libwebsocket *ws, Protocol* const protocol, const long fd);
        
        unsigned int _notify(Connection* conn, enum libwebsocket_callback_reasons const reason,
                             const char* const _message, const unsigned int len);
        
        unsigned int _http(struct libwebsocket *ws, const char* const url);
        
        void setWaitMillis(int millis);
        
    protected:
        std::string     document_root;
        unsigned int    waitMillis;
        std::string     interfaceStr;
        
        bool            bReceivingLargeMessage;
        std::string     largeMessage;
        
        unsigned int    largeBinarySize;
        ofBuffer        largeBinaryMessage;

        bool closeAndFree;
        
        virtual void threadedFunction(){};  
        
        string address;
        string channel;
        int port;
        
        struct libwebsocket_context *context;
        
        std::vector<struct libwebsocket_protocols> lws_protocols;
        
        // maybe this should be a map?
        vector<Connection *> connections;
        
    private:
        Json::Reader reader;
        
    };
    
    extern vector<Reactor *> reactors;
};
