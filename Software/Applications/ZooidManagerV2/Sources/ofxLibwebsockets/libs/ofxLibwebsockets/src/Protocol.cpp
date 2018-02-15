//
//  Protcol.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#include "ofxLibwebsockets/Protocol.h"
#include "ofxLibwebsockets/Reactor.h"

namespace ofxLibwebsockets {

#pragma mark constructor
    
    //--------------------------------------------------------------
    Protocol::Protocol()
    : defaultAllowPolicy(true){
        ofAddListener(onconnectEvent,      this, &Protocol::_onconnect);
        ofAddListener(onopenEvent,         this, &Protocol::_onopen);
        ofAddListener(oncloseEvent,        this, &Protocol::_onclose);
        ofAddListener(onidleEvent,         this, &Protocol::_onidle);
        ofAddListener(onmessageEvent,      this, &Protocol::_onmessage);
        ofAddListener(onerrorEvent,         this, &Protocol::_onerror);
        rx_buffer_size = OFX_LWS_MAX_BUFFER;
        idle = false;
    }

    //--------------------------------------------------------------
    Protocol::~Protocol() {
        ofRemoveListener(onconnectEvent,   this, &Protocol::_onconnect);
        ofRemoveListener(onopenEvent,      this, &Protocol::_onopen);
        ofRemoveListener(oncloseEvent,     this, &Protocol::_onclose);
        ofRemoveListener(onidleEvent,      this, &Protocol::_onidle);
        ofRemoveListener(onmessageEvent,   this, &Protocol::_onmessage);
        ofRemoveListener(onerrorEvent,         this, &Protocol::_onerror);
        rx_buffer_size = OFX_LWS_MAX_BUFFER;
        idle = false;
    }

    //--------------------------------------------------------------
    bool Protocol::_allowClient(const std::string name, const std::string ip) const {
        std::map<std::string, bool>::const_iterator allow_iter;
        
        allow_iter = allowRules.find(name);
        if (allow_iter != allowRules.end())
            return allow_iter->second;
        
        allow_iter = allowRules.find(ip);
        if (allow_iter != allowRules.end())
            return allow_iter->second;
        
        return allowClient(name, ip);
    }

    //--------------------------------------------------------------
    bool Protocol::allowClient(const std::string name, const std::string ip) const {
        return defaultAllowPolicy;
    }
    
#pragma mark events

    //--------------------------------------------------------------
    void Protocol::_onconnect(Event& args){ onconnect(args); }  

    void Protocol::onconnect(Event& args){}

    //--------------------------------------------------------------
    void Protocol::_onopen(Event& args){ onopen(args); }

    void Protocol::onopen(Event&args){}

    //--------------------------------------------------------------
    void Protocol::_onclose(Event& args){ onclose(args); }

    void Protocol::onclose(Event&args){}
    
    //--------------------------------------------------------------
    void Protocol::_onerror(Event& args){ onerror(args); }
    
    void Protocol::onerror(Event&args){}
    
    void Protocol::_onidle(Event& args){
        idle = true;
        onidle(args);
    }

    void Protocol::onidle(Event&args){}

    //--------------------------------------------------------------
    void Protocol::_onmessage(Event& args){
        onmessage(args);
    }

    void Protocol::onmessage(Event&args){}
}
