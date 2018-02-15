#include "ofApp.h"

// utils for gettin' IP address
#ifdef TARGET_OSX
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#endif

//--------------------------------------------------------------
void ofApp::setup(){
    // setup a server with default options on port 9092
    // - pass in true after port to set up with SSL
    //bool connected = server.setup( 9092 );
    
    // Uncomment this to set up a server with a protocol
    // Right now, clients created via libwebsockets that are connecting to servers
    // made via libwebsockets seem to want a protocol. Hopefully this gets fixed, 
    // but until now you have to do something like this:
    
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    
    bool connected = server.setup( options );
    
    // this adds your app as a listener for the server
    server.addListener(this);
    
    // setup message queue
    
    font.load("myriad.ttf", 20);
    messages.push_back("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL") );
    
    ofBackground(0);
    ofSetFrameRate(60);
    bSendImage = false;
    locked = needToLoad = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( bSendImage && toLoad != "" ){
        turbo.load( toLoad, currentImage );
        unsigned long size;
        unsigned char * compressed = turbo.compress(currentImage,100,&size);
        server.sendBinary(compressed, size);
        free(compressed);
        messages.push_back( "Sending image" );
        bSendImage = false;
        toLoad = "";
    }
    
    if ( needToLoad ){
        // you can write this directly to a file!
        //        ofFile test;
        //        test.open("data.jpg", ofFile::WriteOnly);
        //        test.writeFromBuffer(buff);
        //        test.close();
        
        turbo.load(buff, incoming);
        needToLoad = false;
        locked = false;
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int x = font.getSize();
    int y = font.getSize()*2;
    ofSetColor(255);
    for (int i = messages.size() -1; i >= 0; i-- ){
        //font.drawString( messages[i], x, y );
        y += font.stringHeight( messages[i] ) + font.getSize();
    }
    
    // image loaded from drag-and-drop
    if (currentImage.bAllocated()){
        // draw loaded image either at scale or 1/2 the size of window
        // (whichever is smaller)
        float scale = currentImage.getHeight() > ofGetHeight()/2.0 ? (float) ofGetHeight()/2.0/currentImage.getHeight() : 1.0;
        currentImage.draw(0,0, currentImage.getWidth() * scale, currentImage.getHeight() * scale);
    }
    
    // image loaded from incoming binary
    if ( incoming.bAllocated() ){
        // draw image from mobile device either at scale or 1/2 the size of window
        // (whichever is smaller)
        float scale = incoming.getHeight() > ofGetHeight()/2.0 ? (float) ofGetHeight()/2.0/incoming.getHeight() : 1.0;
        incoming.draw(0,ofGetHeight()/2.0, incoming.getWidth() * scale, incoming.getHeight() * scale);
    }
    
    string toDraw = "Drag an image onto the window to send!";
    toDraw += "\nOpen your browser to localhost:9093 to receive\n";
    toDraw += "\nIf you're on the same network, open this URL on your mobile device:";
    
#ifdef TARGET_OSX
    // Below is just me being tricky to make a nice text output.
    // It's from a stack overflow post, of course:
    // http://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine
    // It's NOT NECESSARY to run this example!
    
    static bool bGotIP = false;
    static string myIPaddress = "";

    if ( !bGotIP ){
        bGotIP = true;
        
        struct ifaddrs * ifAddrStruct=NULL;
        struct ifaddrs * ifa=NULL;
        void * tmpAddrPtr=NULL;
        getifaddrs(&ifAddrStruct);
        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
                // is a valid IP4 Address
                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
                
                string addressString = ofToString( addressBuffer);
                if ( addressString != "127.0.0.1"){
                    myIPaddress += "\nhttp://" + addressString +":9093/mobile/index.html";
                }
            } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
                // is a valid IP6 Address
                tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                char addressBuffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
                printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            }
        }
        if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    }
    
    toDraw += myIPaddress;
    // end unnecessary but cool IP address stuff
	
#endif
    
    ofDrawBitmapString(toDraw, 20,20);
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    messages.push_back("New connection from " + args.conn.getClientIP() );
    
    // send the latest image if there is one!
    if ( currentImage.bAllocated() ){
        unsigned long size;
        unsigned char * compressed = turbo.compress(currentImage,100,&size);
        args.conn.sendBinary(compressed, size);
        free(compressed);
    }
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
//    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    if ( args.isBinary ){
        if ( locked ) return;
        // need to load this next frame!
        buff.clear();
        buff.set(args.data.getData(), args.data.size());
        locked = true;
        needToLoad = true;
    } else {
        cout<<"got message "<<args.message<<endl;
        
        // trace out string messages or JSON messages!
        // args.json is null if badly formed or just not JOSN
        if ( !args.json.isNull() ){
            messages.push_back("New message: " + args.json.toStyledString() + " from " + args.conn.getClientName() );
        } else {
            messages.push_back("New message: " + args.message + " from " + args.conn.getClientName() );
        }
        
        //if (messages.size() > NUM_MESSAGES) messages.erase( messages.begin() );
        
        // echo server = send message right back!
        args.conn.send( args.message );
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//-----------------------------------------------------------   ---
void ofApp::mousePressed(int x, int y, int button){
    string url = "http";
    if ( server.usingSSL() ){
        url += "s";
    }
    url += "://localhost:" + ofToString( server.getPort() );
    ofLaunchBrowser(url);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    for (int i=0; i<dragInfo.files.size(); i++){
        string file = dragInfo.files[i];
        ofFile f(file); 
        string ex = f.getExtension();
        std::transform(ex.begin(), ex.end(),ex.begin(), ::toupper);
        
        if ( ex == "JPG" || ex == "JPEG" || ex == "PNG" || ex == "GIF" ){
            toLoad = file;
            bSendImage = true;
        }
    }
}