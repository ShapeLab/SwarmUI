//
//  Util.h
//  ofxLibwebsockets
//
//  This class is primarily the static callbacks needed by libwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once

#include <libwebsockets.h>

#include "ofxLibwebsockets/Connection.h"
#include "ofxLibwebsockets/Reactor.h"
#include "ofxLibwebsockets/Client.h"
#include "ofxLibwebsockets/Server.h"

namespace ofxLibwebsockets {
        
    class Client;
    class Server;
    
    // CLIENT CALLBACK

    extern int lws_client_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len);
    
    // SERVER CALLBACK

    extern int lws_callback(struct libwebsocket_context* context, struct libwebsocket *ws,
                            enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len);

    extern void dump_handshake_info(struct lws_tokens *lwst);
    
    extern string getCallbackReason( int reason );
}
