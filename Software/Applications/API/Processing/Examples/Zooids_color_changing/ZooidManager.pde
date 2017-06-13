import processing.net.*; //<>// //<>// //<>// //<>//
import hypermedia.net.*;

//////////////////////////////////////////////////////////////////
// Written by Mathieu Le Goc - Stanford University - 06-06-17   //
//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
public class Zooid
{

  private int id;
  private float radius;
  private float currentOrientation;
  private float targetOrientation;
  private PVector currentPosition;
  private PVector destination;
  private int state;
  private boolean goalReached;
  private color colour;
  private boolean activated;
  private boolean reassignable;

  //--------------------------------------------------------------
  public Zooid() {
    this.id = 0;
    this.radius = 0.01f;
    this.currentOrientation = 0.0f;
    this.targetOrientation = 0.0f;
    this.currentPosition = new PVector(0.0f, 0.0f, 0.0f);
    this.destination = new PVector(0.0f, 0.0f, 0.0f);
    this.state = 0;
    this.goalReached = true;
    this.colour = color(0);
    this.activated = true;
    this.reassignable = true;
  }

  //--------------------------------------------------------------
  public Zooid(int _id, float _radius, float _orientation, PVector _currentPosition, PVector _destination, int _state, boolean _goalReached, color _colour, boolean _activated, boolean _reassignable) {
    this.id = _id;
    this.radius = _radius;
    this.currentOrientation = _orientation;
    this.targetOrientation = 0.0f;
    this.currentPosition = new PVector(_currentPosition.x, _currentPosition.y);
    this.destination = new PVector(_destination.x, _destination.y);
    this.state = _state;
    this.goalReached = _goalReached;
    this.colour = _colour;
    this.activated = _activated;
    this.reassignable = _reassignable;
  }

  //--------------------------------------------------------------
  public boolean equals(Zooid z) {
    return this.id == z.id;
  }

  //--------------------------------------------------------------
  public void set(Zooid z) {
    this.id = z.id;
    this.radius = z.radius;
    this.targetOrientation = z.currentOrientation;
    this.currentPosition.set(z.currentPosition);
    this.destination.set(z.destination);
    this.state = z.state;
    this.goalReached = z.goalReached;
    this.colour = z.colour;
    this.activated = z.activated;
    this.reassignable = z.reassignable;
  }

  //--------------------------------------------------------------
  public PVector getPosition() {
    return this.currentPosition;
  }

  //--------------------------------------------------------------
  public PVector getDestination() {
    return this.destination;
  }

  //--------------------------------------------------------------
  public float getOrientation() {
    return this.currentOrientation;
  }

  //--------------------------------------------------------------
  public int getState() {
    return this.state;
  }

  //--------------------------------------------------------------
  public boolean isAtDestination() {
    return this.goalReached;
  }

  //--------------------------------------------------------------
  public boolean isReassignable() {
    return this.reassignable;
  }

  //--------------------------------------------------------------
  public int getId() {
    return this.id;
  }

  //--------------------------------------------------------------
  public float getRadius() {
    return this.radius;
  }

  //--------------------------------------------------------------
  public void setDestination(PVector _goal) {
    this.destination.set(_goal);
  }

  //--------------------------------------------------------------
  public void setDestination(float _x, float _y) {
    this.destination.x = _x;
    this.destination.y = _y;
  }

  //--------------------------------------------------------------
  public void setColor(color _colour) {
    this.colour = _colour;
  }

  //--------------------------------------------------------------
  public color getColor() {
    return this.colour;
  }

  //--------------------------------------------------------------
  public void setColor(byte _r, byte _g, byte _b) {
    this.colour = color(_r, _g, _b);
  }

  //--------------------------------------------------------------
  public void setId(int _id) {  
    this.id = _id;
  }

  //--------------------------------------------------------------
  public void setTouch(byte _state) {
    this.state = _state;
  }

  //--------------------------------------------------------------
  public void setOrientation(float _orientation) {
    this.targetOrientation = _orientation;
  }

  //--------------------------------------------------------------
  public void setReassignable(boolean _reassignable) {
    this.reassignable = _reassignable;
  }

  //--------------------------------------------------------------
  public void activate() {
    this.activated = true;
  }

  //--------------------------------------------------------------
  public void deactivate() {
    this.activated = false;
  }

  //--------------------------------------------------------------
  public boolean isActivated() {
    return this.activated  ;
  }

  //--------------------------------------------------------------
  public void printInfo() {
    print(this.getId());
    print("\t");
    print(this.getRadius());
    print("\t");
    print(this.getOrientation());
    print("\t");
    print(this.getPosition());
    print("\t");
    print(this.getDestination());
    print("\t");
    print(this.getState());
    print("\t");
    print(this.isAtDestination());
    print("\t");
    print(this.getColor());
    print("\t");
    print(this.isActivated());
    print("\t");
    println(this.isReassignable());
  }

  //--------------------------------------------------------------
  public boolean isTouched() {
    return (state & (1 << 0)) > 0;
  }

  //--------------------------------------------------------------
  public boolean isBlinded() {
    return (state & (1 << 1)) > 0;
  }

  //--------------------------------------------------------------
  public boolean isTapped() {
    return (state & (1 << 2)) > 0;
  }

  //--------------------------------------------------------------
  public boolean isShaken() {
    return (state & (1 << 3)) > 0;
  }
}

//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//
//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
public class ZooidManager
{
  private UDP udp;
  private ArrayList<Zooid> myZooids;
  private int assignmentMode;

  public final int OptimalAssignment = 1;
  public final int NaiveAssignment = 0;
  private float dimensionX;
  private float dimensionY;
  private boolean initialized;
  private boolean realCoordinates;

  private float windowWidth, windowHeight;

  //--------------------------------------------------------------
  public ZooidManager() {
    assignmentMode = OptimalAssignment;
    myZooids = new ArrayList<Zooid>();
    initialized = false;
    realCoordinates = false;
  }

  //--------------------------------------------------------------
  public void initialize(float windowWidth, float windowHeight) {
    this.windowWidth = windowWidth; 
    this.windowHeight = windowHeight;

    udp = new UDP( this, 11999);
    udp.listen( true );
    udp.setBuffer(10000);
  }

  //--------------------------------------------------------------
  public boolean sendUpdates() {

    JSONObject root = new JSONObject();

    root.setInt("ass", assignmentMode);
    root.setInt("nb", myZooids.size());

    JSONArray zooids = new JSONArray();

    for (int i = 0; i < myZooids.size(); i++) {
      JSONObject z = new JSONObject();

      z.setInt("id", myZooids.get(i).getId());
      z.setFloat("ang", myZooids.get(i).targetOrientation);

      JSONArray dest = new JSONArray();
      dest.setFloat(0, myZooids.get(i).getDestination().x);
      dest.setFloat(1, myZooids.get(i).getDestination().y);
      z.setJSONArray("des", dest);

      JSONArray col = new JSONArray();
      col.setInt(0, int(red(myZooids.get(i).getColor())));
      col.setInt(1, int(green(myZooids.get(i).getColor())));
      col.setInt(2, int(blue(myZooids.get(i).getColor())));
      z.setJSONArray("col", col);

      z.setBoolean("act", myZooids.get(i).isActivated());
      z.setBoolean("rea", myZooids.get(i).isReassignable());

      zooids.setJSONObject(i, z);
    }
    root.setJSONArray("zoo", zooids);

    return udp.send(root.toString(), "localhost", 11998);
  } 

  //--------------------------------------------------------------
  public void receive( byte[] data ) {       // <-- default handler
    String s = new String(data);
    JSONObject zooidData = JSONObject.parse(s);


    if (zooidData == null) {
      println("JSONObject could not be parsed");
    } else {
      int nbZooids = zooidData.getInt("nb");
      assignmentMode = zooidData.getInt("ass");
      if (!initialized) {
        JSONArray dim = zooidData.getJSONArray("dim");
        dimensionX = dim.getFloat(0);
        dimensionY = dim.getFloat(1);
        initialized = true;
      }

      JSONArray receivedZooids = zooidData.getJSONArray("zoo");

      if (receivedZooids.size() == nbZooids) {
        for (int i = 0; i < receivedZooids.size(); i++) {
          JSONObject z = receivedZooids.getJSONObject(i);

          JSONArray posArray = z.getJSONArray("pos");
          PVector pos = new PVector(posArray.getFloat(0), posArray.getFloat(1));
          JSONArray destArray = z.getJSONArray("des");
          PVector dest = new PVector(destArray.getFloat(0), destArray.getFloat(1));
          JSONArray colArray = z.getJSONArray("col");
          color col = color(colArray.getInt(0), colArray.getInt(1), colArray.getInt(2));

          Zooid tmpZooid;
          tmpZooid = new Zooid(z.getInt("id"), 
            z.getFloat("siz")*1.5f, 
            z.getFloat("ang"), 
            pos, 
            dest, 
            z.getInt("sta"), 
            z.getBoolean("her"), 
            col, 
            z.getBoolean("act"), 
            z.getBoolean("rea"));

          boolean zooidFound = false;

          for (int j = 0; j < myZooids.size(); j++)
          {
            if (myZooids.get(j).getId() == tmpZooid.getId()) {
              myZooids.get(j).set(tmpZooid);
              zooidFound = true;
              break;
            }
          }
          if (!zooidFound) {
            myZooids.add(tmpZooid);
          }
        }
      }
    }
  }

  //--------------------------------------------------------------
  public void setZooidColor(int id, color c) {
    if (id < myZooids.size())
      myZooids.get(id).setColor(c);
  }

  //--------------------------------------------------------------
  public color getZooidColor(int id) {
    color c = 0;
    if (id < myZooids.size()) {
      c = myZooids.get(id).getColor();
    }
    return c;
  }

  //--------------------------------------------------------------
  public void moveZooid(int id, float x, float y) {
    if (id < myZooids.size()) {
      PVector p = new PVector();
      if (realCoordinates) 
        p .set(map(x, 0.0f, dimensionX, dimensionX, 0.0f), map(y, 0.0f, dimensionY, dimensionY, 0.0f));
      else
        p.set(map(x, 0.0f, windowWidth, dimensionX, 0.0f), map(y, 0.0f, windowHeight, dimensionY, 0.0f));

      myZooids.get(id).setDestination(p);
    }
  }

  //--------------------------------------------------------------
  public void moveZooid(int id, PVector pos) {
    if (id < myZooids.size()) {
      PVector p = new PVector();
      if (realCoordinates) 
        p .set(map(pos.x, 0.0f, dimensionX, dimensionX, 0.0f), map(pos.y, 0.0f, dimensionY, dimensionY, 0.0f));
      else
        p.set(map(pos.x, 0.0f, windowWidth, dimensionX, 0.0f), map(pos.y, 0.0f, windowHeight, dimensionY, 0.0f));
      myZooids.get(id).setDestination(p);
    }
  }

  //--------------------------------------------------------------
  public PVector getZooidPosition(int id) {
    PVector p = new PVector(0.0f, 0.0f);
    if (id < myZooids.size()) {
      if (realCoordinates) 
        p .set(map(myZooids.get(id).getPosition().x, dimensionX, 0.0f, 0.0f, dimensionX), map(myZooids.get(id).getPosition().y, dimensionY, 0.0f, 0.0f, dimensionY));
      else
        p.set(map(myZooids.get(id).getPosition().x, dimensionX, 0.0f, 0.0f, windowWidth), map(myZooids.get(id).getPosition().y, dimensionY, 0.0f, 0.0f, windowHeight));
    }
    return p;
  }

  //--------------------------------------------------------------
  public float getZooidOrientation(int id) {
    if (id < myZooids.size()) 
      return myZooids.get(id).getOrientation();
    else
      return 0.0f;
  }

  //--------------------------------------------------------------
  public void setZooidOrientation(int id, float angle) {
    if (id < myZooids.size()) 
      myZooids.get(id).setOrientation(angle);
  }

  //--------------------------------------------------------------
  public int getNbZooids() {
    return myZooids.size();
  }

  //--------------------------------------------------------------
  public boolean isZooidTouched(int id) {
    if (id < myZooids.size()) 
      return myZooids.get(id).isTouched();
    else
      return false;
  }

  //--------------------------------------------------------------
  public boolean isZooidBlinded(int id) {
    if (id < myZooids.size()) 
      return myZooids.get(id).isBlinded();
    else
      return false;
  }

  //--------------------------------------------------------------
  public boolean isZooidTapped(int id) {
    if (id < myZooids.size()) 
      return myZooids.get(id).isTapped();
    else
      return false;
  }

  //--------------------------------------------------------------
  public boolean isZooidShaken(int id) {
    if (id < myZooids.size()) 
      return myZooids.get(id).isShaken();
    else
      return false;
  }

  //--------------------------------------------------------------
  public boolean isInitialized() {
    return initialized;
  }

  //--------------------------------------------------------------
  public int getAssignmentMode() {
    return assignmentMode;
  }

  //--------------------------------------------------------------
  public void setAssignementMode(int mode) {
    if (mode == NaiveAssignment || mode == OptimalAssignment) 
      assignmentMode = mode;
  }

  //--------------------------------------------------------------
  public float getZooidSize() {
    float size = 1.0f;
    if (myZooids.size()>0) {
      if (realCoordinates) 
        size = 2.0f * myZooids.get(0).getRadius();
      else
        size = 2.0f*map(myZooids.get(0).getRadius(), 0.0f, dimensionX, 0.0f, windowWidth);
    }
   
      return size;
  }

  //--------------------------------------------------------------
  public void setZooidReassignable(int id, boolean _reassignable) {
    if (id < myZooids.size()) 
      myZooids.get(id).setReassignable(_reassignable);
  }

  //--------------------------------------------------------------
  public void activateZooid(int id) {
    if (id < myZooids.size()) 
      myZooids.get(id).activate();
  }

  //--------------------------------------------------------------
  public void deactivateZooid(int id) {
    if (id < myZooids.size()) 
      myZooids.get(id).deactivate();
  }

  //--------------------------------------------------------------
  public void useRealCoordinates() {
    realCoordinates = true;
  }

  //--------------------------------------------------------------
  public void useWindowCoordinates() {
    realCoordinates = false;
  }

  //--------------------------------------------------------------
  public void setWindowSize(float w, float h) {
    windowWidth = w;
    windowHeight = h;
  }
}