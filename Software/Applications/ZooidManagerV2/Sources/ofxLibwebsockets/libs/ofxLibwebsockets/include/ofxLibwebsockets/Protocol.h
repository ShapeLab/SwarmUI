//
//  Protocol.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once

#include <string>
#include <map>

#include "ofMain.h"
#include "ofxLibwebsockets/Events.h"

#define OFX_LWS_MAX_BUFFER 2048

namespace ofxLibwebsockets {
    
    class Reactor;

    #include "json.h"

    class Protocol
    {
        friend class Reactor;
        friend class Server;
        friend class Client;
        friend class Connection;
        
    public:
        Protocol();
        ~Protocol();
        
        virtual bool allowClient(const std::string name,
                                 const std::string ip) const;
        
        unsigned int idx;
        unsigned int rx_buffer_size;
        
    protected:  
        // override these methods if/when creating
        // a custom protocol
        virtual void execute() {}
        
        virtual void onconnect  (Event& args);
        virtual void onopen     (Event& args);
        virtual void onclose    (Event& args);
        virtual void onerror    (Event& args);
        virtual void onidle     (Event& args);
        virtual void onmessage  (Event& args);
        
        // internal events: called by Reactor
        ofEvent<Event> onconnectEvent;
        ofEvent<Event> onopenEvent;
        ofEvent<Event> oncloseEvent;
        ofEvent<Event> onerrorEvent;
        ofEvent<Event> onidleEvent;
        ofEvent<Event> onmessageEvent;
        
        bool defaultAllowPolicy;
        std::map<std::string, bool> allowRules;
        
        Reactor* reactor;
        
        bool idle;
        
    private:
        void _onconnect   (Event& args);
        void _onopen      (Event& args);
        void _onclose     (Event& args);
        void _onerror     (Event& args);
        void _onidle      (Event& args);
        void _onmessage   (Event& args);
        
        bool _allowClient(const std::string name,
                          const std::string ip) const;
    };
};