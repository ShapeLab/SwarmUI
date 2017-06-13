boolean firstRun = true;
ZooidManager zooidManager = new ZooidManager();
int step = 0;

void setup() 
{
  size(1024, 750);
  zooidManager.initialize(width, height);
  zooidManager.useWindowCoordinates();
}  

void UpdateZooids() {

  if (zooidManager.isInitialized()) {
    for (int i=0; i<zooidManager.getNbZooids(); i++) {
      zooidManager.setZooidOrientation(i, (float)(step%360)/10.0f);
      if (i%3==0)
        zooidManager.setZooidColor(i, color(step%255, 0, 0));
      else if (i%3==1)
        zooidManager.setZooidColor(i, color(0, step%255, 0));
      else if (i%3==2)
        zooidManager.setZooidColor(i, color(0, 0, step%255));

      if (i%2==0)
        zooidManager.deactivateZooid(i);
      else
        zooidManager.activateZooid(i);

      zooidManager.setZooidReassignable(i, (i%2==0)?true:false);
    }

    zooidManager.sendUpdates();
  }
}

void Update() {

  UpdateZooids();
  step++;
  ////send the positions and colors of the points
}

void draw() {    
  Update();
  background(220);  
  noStroke();

  if (!zooidManager.isInitialized()) {
    //drawString
  }
  
  for (int i=0; i<zooidManager.getNbZooids(); i++) {
    fill(zooidManager.getZooidColor(i));
    ellipse(zooidManager.getZooidPosition(i).x, 
      zooidManager.getZooidPosition(i).y, 
      zooidManager.getZooidSize(), 
      zooidManager.getZooidSize());
  }
}

void mouseMoved() {
}

void mousePressed() {
}

void mouseDragged() {
}

void mouseReleased() {
}