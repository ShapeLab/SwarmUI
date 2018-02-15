#ifndef RobotGoal_h
#define RobotGoal_h

#include "ofMain.h"
#include "ofConstants.h"
#include "ofxSvg.h"

class Zooid;

class ZooidGoal
{
private:
	unsigned int id;
	ofVec2f position;
    float orientation;
	ofColor color;
	Zooid* zooid;
    bool reassignable;

public:
	ZooidGoal();
	~ZooidGoal();
	ZooidGoal(ofVec2f _position, float _orientation, ofColor _color);
	void operator = (const ZooidGoal &g);

	void setId(unsigned int _id);
	void setPosition(ofVec2f _position);
	void setPosition(float _x, float _y);
    void setOrientation(float _orientation);
	void setColor(ofColor _color);
	void setAssociatedZooid(Zooid *_zooid);
    void setReassignable(bool _reassignable);

	unsigned int getId();
	ofVec2f getPosition();
    float getOrientation();
	ofColor getColor();
	Zooid* getAssociatedZooid();
    bool isReassignable();

	void drawGoal(float size);
    void drawLock(ofxSVG *artwork, float goalSize, float scale);

};

#endif //RobotGoal
