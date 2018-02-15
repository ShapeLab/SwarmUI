//
//  Events.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#include "ofxLibwebsockets/Events.h"

namespace ofxLibwebsockets {
        
    //--------------------------------------------------------------
    Event::Event(Connection& _conn, std::string _message, bool isBinary)
    : conn(_conn)
    , message(_message)
    , isBinary(isBinary)
    {}
}