//
//  Client.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#pragma once

#include <libwebsockets.h>
#include "ofxLibwebsockets/Reactor.h"

namespace ofxLibwebsockets {

    struct ClientOptions {
        string  host;
        int     port;
        bool    bUseSSL;
        string  channel;
        string  protocol;
        int     version;
        bool    reconnect;
        int     reconnectInterval;
        
        // advanced: timeout options
        // names are from libwebsockets (ka == keep alive)
        int     ka_time;        // 0 == default, no timeout; nonzero == time to wait in seconds before testing conn
        int     ka_probes;      // # of times to test for connection; ignored if ka_time == 0
        int     ka_interval;    // how long to wait between probes, in seconds; ignored if ka_time == 0
    };
    
    // call this function to set up a vanilla client options object
    extern ClientOptions defaultClientOptions();
    
    class Client : public Reactor {
        friend class Protocol;
    public:  
        
        Client();
        ~Client();

        // Note: the boolean returned here == libwebsockets setup success
        // You will receive an "onOpen" event on successful connect
        // and "onClose" on unsuccessful
        bool connect ( string _address, bool bUseSSL=false );
        bool connect ( string _address, int _port, bool bUseSSL=false );
        bool connect ( ClientOptions options );
        
        // force close this client
        void close();
        
        // check if connected
        bool isConnected();
        
        // most basic send function
        void send( string message );
        
        // send anything that has pixels
        template <class T>
        void sendBinary( T& image ){
            if ( connection != NULL ){
                int size = image.getWidth() * image.getHeight() * image.getPixels().getNumChannels();
                connection->sendBinary( (char *) image.getPixels().getData(), size );
            }
        }
        
        // send any binary data
        void sendBinary( ofBuffer & buffer );
        void sendBinary( unsigned char * data, int size );
        void sendBinary( char * data, int size );
        
        template<class T>
        void addListener(T * app){
            ofAddListener( clientProtocol.onconnectEvent, app, &T::onConnect); 
            ofAddListener( clientProtocol.onopenEvent, app, &T::onOpen);
            ofAddListener( clientProtocol.oncloseEvent, app, &T::onClose);
            ofAddListener( clientProtocol.onidleEvent, app, &T::onIdle);
            ofAddListener( clientProtocol.onmessageEvent, app, &T::onMessage);
        }
        
        // get pointer to libwebsockets connection wrapper
        Connection * getConnection(){
            return connection;
        }

        // Note: you do not need to call this function! It is called
        // automatically and only used for reconnecting
        void update(ofEventArgs& args);

    protected:
        ClientOptions defaultOptions;
        void onClose( Event& args );
        
    private:

        Connection * connection;
        
        struct libwebsocket *  lwsconnection;
        void threadedFunction();
        
        //wrap protocol
        Protocol clientProtocol;

        bool bShouldReconnect;
        uint64_t lastReconnectTime;
        
    };
};
