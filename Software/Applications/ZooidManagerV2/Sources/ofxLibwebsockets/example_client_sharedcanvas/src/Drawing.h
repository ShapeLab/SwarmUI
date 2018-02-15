//
//  Drawing.h
//  SharedCanvas
//
//  Created by Brett Renfer on 5/29/12. 
//

class Drawing {
public:
    int             _id;
    ofColor         color;
    vector<ofPoint> points;
    ofxLibwebsockets::Connection * conn;
    
    Drawing(){
        conn = new ofxLibwebsockets::Connection(); _id = -1;
    }
    
    void addPoint( ofPoint p ){
        if ( points.size() + 1 > 500 ){
            points.erase( points.begin() );
        }
        points.push_back( p );
    }
    
    string getColorJSON( bool object=false){
        int r = color.r;
        int g = color.g;
        int b = color.b;
        
        stringstream jsonstring;
        if ( object ){
            jsonstring<<"{\"color\":{\"r\":\""<<r<<"\",\"g\":\""<<g<<"\",\"b\":\""<<b<<"\"}}";   
        } else {
            jsonstring<<"\"color\":{\"r\":\""<<r<<"\",\"g\":\""<<g<<"\",\"b\":\""<<b<<"\"}";
        }
        return jsonstring.str();
    }
    
    string getJSONString( string wrapper){
        
        stringstream jsonstring;
        if ( wrapper != "" ){
            jsonstring << "{\"" + wrapper << "\":";
        }
        jsonstring<<"{" +getColorJSON()+",\"id\":\""<< _id<<"\"}";
        
        if ( wrapper != "" ){
            jsonstring << "}";
        }
        return jsonstring.str();
    }
};