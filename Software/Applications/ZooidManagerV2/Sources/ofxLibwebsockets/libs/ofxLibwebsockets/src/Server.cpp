//
//  Server.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//

#include "ofxLibwebsockets/Server.h"
#include "ofxLibwebsockets/Util.h"

#include "ofEvents.h"
#include "ofUtils.h"

namespace ofxLibwebsockets {

	ServerOptions defaultServerOptions(){
        ServerOptions opts;
        opts.port           = 80;
        opts.protocol       = "NULL"; // NULL == no protocol. most websockets behave this way.
        opts.bUseSSL        = false;
        opts.sslCertPath    = ofToDataPath("ssl/libwebsockets-test-server.pem", true);
        opts.sslKeyPath     = ofToDataPath("ssl/libwebsockets-test-server.key.pem", true);
        opts.documentRoot   = ofToDataPath("web", true);
        opts.ka_time        = 0;
        opts.ka_probes      = 0;
        opts.ka_interval    = 0;
        return opts;
    }

    //--------------------------------------------------------------
    Server::Server(){
        context = NULL;
        waitMillis = 1;
        reactors.push_back(this);
        
        defaultOptions = defaultServerOptions();
    }
    
    //--------------------------------------------------------------
    Server::~Server(){
        exit();
    }

    //--------------------------------------------------------------
    bool Server::setup( int _port, bool bUseSSL )
    {
        // setup with default protocol (http) and allow ALL other protocols
        defaultOptions.port     = _port;
        defaultOptions.bUseSSL  = bUseSSL;
        
        if ( defaultOptions.port == 80 && defaultOptions.bUseSSL == true ){
            ofLog( OF_LOG_WARNING, "[ofxLibwebsockets] SSL IS NOT USUALLY RUN OVER DEFAULT PORT (80). THIS MAY NOT WORK!");
        }
        
        return setup( defaultOptions );
    }

    //--------------------------------------------------------------
    bool Server::setup( ServerOptions options ){
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
		lws_set_log_level(LLL_WARN, NULL);

        defaultOptions = options;
        
        port = defaultOptions.port = options.port;
        document_root = defaultOptions.documentRoot = options.documentRoot;
        
        // NULL protocol is required by LWS
        struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
        
        //setup protocols
        lws_protocols.clear();
        
        //register main protocol
        registerProtocol( options.protocol, serverProtocol );
        
        //register any added protocols
        for (int i=0; i<protocols.size(); ++i){
            struct libwebsocket_protocols lws_protocol = {
                ( protocols[i].first == "NULL" ? NULL : protocols[i].first.c_str() ),
                lws_callback,
                protocols[i].second->rx_buffer_size,
                protocols[i].second->rx_buffer_size
            };
            lws_protocols.push_back(lws_protocol);
        }
        lws_protocols.push_back(null_protocol);
        
        // make cert paths  null if not using ssl
        const char * sslCert = NULL;
        const char * sslKey = NULL;
        
        if ( defaultOptions.bUseSSL ){
            sslCert = defaultOptions.sslCertPath.c_str();
            sslKey = defaultOptions.sslKeyPath.c_str();
        }
        
        int opts = 0;
        struct lws_context_creation_info info;
        memset(&info, 0, sizeof info);
        info.port = port;
        info.protocols = &lws_protocols[0];
        info.extensions = libwebsocket_get_internal_extensions();
        info.ssl_cert_filepath = sslCert;
        info.ssl_private_key_filepath = sslKey;
        info.gid = -1;
        info.uid = -1;
        
        if ( options.ka_time != 0 ){
            info.ka_time = options.ka_time;
            info.ka_probes = options.ka_probes;
            info.ka_interval = options.ka_interval;
        }
        
        info.options = opts;

        context = libwebsocket_create_context(&info);
        
        if (context == NULL){
            ofLogError() << "[ofxLibwebsockets] libwebsockets init failed";
            return false;
        } else {
            startThread(); // blocking, non-verbose        
            return true;
        }
    }
    
    //--------------------------------------------------------------
	void Server::close() {
        if (isThreadRunning()){
            // this is the strategy from ofxKinect
            stopThread();
            ofSleepMillis(10);
            waitForThread(false);
        }
		libwebsocket_context_destroy(context);
	}
    
    //--------------------------------------------------------------
    void Server::send( string message ){
        for (int i=0; i<connections.size(); i++){
            if ( connections[i] ){
                connections[i]->send( message );
            }
        }
    }
    
    //--------------------------------------------------------------
    void Server::sendBinary( ofBuffer & buffer ){
        sendBinary(buffer.getData(), buffer.size());
    }
    
    //--------------------------------------------------------------
    void Server::sendBinary( unsigned char * data, int size ){
        sendBinary(reinterpret_cast<char *>(data), size);
    }
    
    //--------------------------------------------------------------
    void Server::sendBinary( char * data, int size ){
        for (int i=0; i<connections.size(); i++){
            if ( connections[i] ){
                connections[i]->sendBinary( data, size );
            }
        }
    }
    
    //--------------------------------------------------------------
    void Server::send( string message, string ip ){
        bool bFound = false;
        for (int i=0; i<connections.size(); i++){
            if ( connections[i] ){
                if ( connections[i]->getClientIP() == ip ){
                    connections[i]->send( message );
                    bFound = true;
                }
            }
        }
        if ( !bFound ) ofLog( OF_LOG_ERROR, "[ofxLibwebsockets] Connection not found at this IP!" );
    }
    
    //getters
    //--------------------------------------------------------------
    int Server::getPort(){
        return defaultOptions.port;
    }
    
    //--------------------------------------------------------------
    string Server::getProtocol(){
        return ( defaultOptions.protocol == "NULL" ? "none" : defaultOptions.protocol );
    }
    
    //--------------------------------------------------------------
    bool Server::usingSSL(){
        return defaultOptions.bUseSSL;
    }

    //--------------------------------------------------------------
    void Server::threadedFunction()
    {
        while (isThreadRunning())
        {
            // update all connections
            for (int i=0; i<connections.size(); i++){
                if ( connections[i] ){
                    connections[i]->update();
                }
            }
            for (int i=0; i<protocols.size(); ++i){
                if (protocols[i].second != NULL){
                    //lock();
                    protocols[i].second->execute();
                    //unlock();
                }
            }
            
            if (lock())
            {
                libwebsocket_service(context, waitMillis);
                unlock();
            }
            yield();
        }
    }
}
