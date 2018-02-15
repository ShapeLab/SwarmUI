//
//  Client.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#include "ofxLibwebsockets/Client.h"
#include "ofxLibwebsockets/Util.h"

namespace ofxLibwebsockets {

   ClientOptions defaultClientOptions(){
       ClientOptions opts;
       opts.host      = "localhost";
       opts.port      = 80;
       opts.bUseSSL   = false;
       opts.channel   = "/";
       opts.protocol  = "NULL";
       opts.version   = -1;     //use latest version

       // Note, turning this on has been seen to cause an EXC_BAD_ACCESS error
       // when calling libwebsocket_service() in threadedFunction(). If you're
       // having issues, try raising the reconnect interval, or not using this
       // reconnect option. Use at your own risk!
       opts.reconnect = false;
       opts.reconnectInterval = 1000;

       opts.ka_time      = 0;
       opts.ka_probes    = 0;
       opts.ka_interval  = 0;
       return opts;
   };

    Client::Client(){
        context = NULL;
        connection = NULL;
        waitMillis = 1;
        reactors.push_back(this);
        
        defaultOptions = defaultClientOptions();

        ofAddListener( ofEvents().update, this, &Client::update);
        ofAddListener( clientProtocol.oncloseEvent, this, &Client::onClose);
    }

    
    //--------------------------------------------------------------
    Client::~Client(){
        exit();
        ofRemoveListener( ofEvents().update, this, &Client::update);
    }
    
    //--------------------------------------------------------------
    bool Client::connect ( string _address, bool bUseSSL ){
        defaultOptions.host     = _address;
        defaultOptions.bUseSSL  = bUseSSL;
        defaultOptions.port     = (bUseSSL ? 443 : 80);
        
        return connect( defaultOptions );
    }

    //--------------------------------------------------------------
    bool Client::connect ( string _address, int _port, bool bUseSSL ){
        defaultOptions.host     = _address;
        defaultOptions.port     = _port;
        defaultOptions.bUseSSL  = bUseSSL;
        
        return connect( defaultOptions );
    }

    //--------------------------------------------------------------
    bool Client::connect ( ClientOptions options ){
        ofLog( OF_LOG_VERBOSE, "[ofxLibwebsockets] connect: "+options.host+":"+ofToString(options.port)+options.channel+":"+ofToString(options.bUseSSL) );
        address = options.host;
        port    = options.port;  
        channel = options.channel;
        defaultOptions = options;
        bShouldReconnect = defaultOptions.reconnect;

		/*
			enum lws_log_levels {
			LLL_ERR = 1 << 0,
			LLL_WARN = 1 << 1,
			LLL_NOTICE = 1 << 2,
			LLL_INFO = 1 << 3,
			LLL_DEBUG = 1 << 4,
			LLL_PARSER = 1 << 5,
			LLL_HEADER = 1 << 6,
			LLL_EXT = 1 << 7,
			LLL_CLIENT = 1 << 8,
			LLL_LATENCY = 1 << 9,
			LLL_COUNT = 10 
		};
		*/
		lws_set_log_level(LLL_ERR, NULL);

        // set up default protocols
        struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
        
        // setup the default protocol (the one that works when you do addListener())
        registerProtocol( options.protocol, clientProtocol );  
        
        lws_protocols.clear();
        for (int i=0; i<protocols.size(); ++i)
        {
            struct libwebsocket_protocols lws_protocol = {
                ( protocols[i].first == "NULL" ? NULL : protocols[i].first.c_str() ),
                lws_client_callback,
                protocols[i].second->rx_buffer_size,
                protocols[i].second->rx_buffer_size
            };
            lws_protocols.push_back(lws_protocol);
        }
        lws_protocols.push_back(null_protocol);

        struct lws_context_creation_info info;
        memset(&info, 0, sizeof info);
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = &lws_protocols[0];
        info.extensions = libwebsocket_get_internal_extensions();
        info.gid = -1;
        info.uid = -1;
        
        if ( options.ka_time != 0 ){
            ofLogVerbose()<<"[ofxLibwebsockets] Setting timeout "<<options.ka_time;
            info.ka_time = options.ka_time;
            info.ka_probes = options.ka_probes;
            info.ka_interval = options.ka_interval;
        }

        context = libwebsocket_create_context(&info);

        
        //context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
        //                                      &lws_protocols[0], libwebsocket_internal_extensions,
        //                                      NULL, NULL, /*NULL,*/ -1, -1, 0, NULL);
        if (context == NULL){
            ofLogError() << "[ofxLibwebsockets] libwebsocket init failed";
            return false;
        } else {
            ofLogVerbose() << "[ofxLibwebsockets] libwebsocket init success";
            
            string host = options.host +":"+ ofToString( options.port );
            
            // register with or without a protocol
            if ( options.protocol == "NULL"){
                lwsconnection = libwebsocket_client_connect( context, 
                                                            options.host.c_str(), options.port, (options.bUseSSL ? 2 : 0 ),
                                                            options.channel.c_str(), host.c_str(), host.c_str(), NULL, options.version);
            } else {
                lwsconnection = libwebsocket_client_connect( context, 
                                                            options.host.c_str(), options.port, (options.bUseSSL ? 2 : 0 ), 
                                                            options.channel.c_str(), host.c_str(), host.c_str(), options.protocol.c_str(), options.version);
            }
                        
            if ( lwsconnection == NULL ){
                ofLogError() << "[ofxLibwebsockets] client connection failed";
                return false;
            } else {
                connection = new Connection( (Reactor*) &context, &clientProtocol );
                connection->ws = lwsconnection;
                
                ofLogVerbose() << "[ofxLibwebsockets] Connection successfully created. Connecting.";
                startThread();   
                return true;
            }
        }
    }
    
    //--------------------------------------------------------------
    bool Client::isConnected(){
        if ( connection == NULL || lwsconnection == NULL ){
            return false;
        } else {
            // we need a better boolean switch...
            // for now, connections vector is only populated
            // with valid connections
            return connections.size() > 0;
        }
    }

    //--------------------------------------------------------------
    void Client::close(){
        // Self-initiated call to close() means we shouldn't try to reconnect
        bShouldReconnect = false;

        if (isThreadRunning()){
            waitForThread(true);
        } else {
			return;
		}
        if ( context != NULL ){
            //libwebsocket_close_and_free_session( context, lwsconnection, LWS_CLOSE_STATUS_NORMAL);
            closeAndFree = true;
            libwebsocket_context_destroy( context );
            context = NULL;        
            lwsconnection = NULL;
        }
		if ( connection != NULL){
                //delete connection;
			connection = NULL;                
		}
    }


    //--------------------------------------------------------------
    void Client::onClose( Event& args ){
		// on windows an exit of the server let's the client crash
		// the event is called from the processing of the thread
		// thus all we can do wait for the thread to stop itself
		// by detecting that lwsconnection is NULL
		if ( context != NULL ){
			closeAndFree = true;
			lwsconnection = NULL;
		}

		lastReconnectTime = ofGetElapsedTimeMillis();
    }

    //--------------------------------------------------------------
    void Client::send( string message ){
        if ( connection != NULL){
            connection->send( message );
        }
    }
    
    //--------------------------------------------------------------
    void Client::sendBinary( ofBuffer & buffer ){
        if ( connection != NULL){
            connection->sendBinary(buffer);
        }
    }
    
    //--------------------------------------------------------------
    void Client::sendBinary( unsigned char * data, int size ){
        if ( connection != NULL){
            connection->sendBinary(data,size);
        }
    }
    
    //--------------------------------------------------------------
    void Client::sendBinary( char * data, int size ){
        if ( connection != NULL){
            connection->sendBinary(data,size);
        }
    }

    //--------------------------------------------------------------
    void Client::update(ofEventArgs& args) {
        if (!isConnected() && bShouldReconnect) {
            uint64_t now = ofGetElapsedTimeMillis();
            if (now - lastReconnectTime > defaultOptions.reconnectInterval) {
                lastReconnectTime = now;
                connect( defaultOptions );
            }
        }
    }

    //--------------------------------------------------------------
    void Client::threadedFunction(){
        while ( isThreadRunning() ){
            for (int i=0; i<protocols.size(); ++i){
                if (protocols[i].second != NULL){
                    //lock();
                    protocols[i].second->execute();
                    //unlock();
                }
            }
            if (context != NULL && lwsconnection != NULL){
                //libwebsocket_callback_on_writable(context,lwsconnection);
                connection->update();
                
                if (lock())
                {
                    int n = libwebsocket_service(context, waitMillis);
                    unlock();
                }
                yield();
            } else {
				stopThread();
				if ( context != NULL ){
					closeAndFree = true;
					libwebsocket_context_destroy( context );
					context = NULL;        
					lwsconnection = NULL;
				}
				if (connection != NULL){
					connection = NULL;                
				}
            }
        }
    }
}
