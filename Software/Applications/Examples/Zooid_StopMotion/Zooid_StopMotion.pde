boolean firstRun = true;
ZooidManager zooidManager = new ZooidManager();
int step = 0;

ArrayList<Point[]> frames;
int currentIndex = -1;
boolean playing = false;

long playingTime;
int timestep = 1000;
float speed = 1.0f;

boolean buttonLClicked = false, buttonRClicked = false;
int rectLX, rectLY;    
int rectRX, rectRY;
int rectW, rectH;
PImage backgroundImg;


void setup() 
{
  size(1024, 750);
  frameRate(60);
  //pixelDensity(displayDensity());
  zooidManager.initialize(width, height);
  zooidManager.useWindowCoordinates();

  frames = new ArrayList<Point[]>();
  playingTime = millis();

  selectInput("Select the file to load:", "load");

  rectLX = width/2-80;
  rectLY = height - 50;
  rectRX = width/2+80;
  rectRY = height - 50;
  rectW = 120;
  rectH = 80;

  backgroundImg = loadImage(sketchPath()+"/data/background.jpg");
  if (backgroundImg != null)
    backgroundImg.resize(width, height);
}  

void exit() {
  selectFolder("Select a folder to save:", "save");
}

void UpdateZooids() {

  if (zooidManager.isInitialized()) {
    for (int i=0; i<zooidManager.getNbZooids(); i++) {
      zooidManager.setZooidReassignable(i, false);
      if (frames.size()>0) {
        if (zooidManager.isZooidTouched(i)) 
          frames.get(currentIndex)[i].setPosition(zooidManager.getZooidPosition(i));
        zooidManager.moveZooid(i, frames.get(currentIndex)[i].getPosition());
      } else {
        addFrame();
        currentIndex++;
      }
    }
    zooidManager.sendUpdates();
  }
}

void Update() {
  if (playing && millis() - playingTime > timestep) {
    currentIndex = (currentIndex+1)%frames.size();
    playingTime = millis();
  }
  UpdateZooids();
}

boolean addFrame() {
  if (zooidManager.isInitialized() && zooidManager.getNbZooids()>0) {
    Point[] points = new Point[zooidManager.getNbZooids()];
    for (int i=0; i<zooidManager.getNbZooids(); i++) 
      points[i] = new Point(i, zooidManager.getZooidPosition(i), zooidManager.getZooidColor(i));

    frames.add(points);
    return true;
  }
  return false;
}

boolean removeFrame(int index)
{
  if (index<frames.size()) {
    frames.remove(index);
    return true;
  }
  return false;
}

void drawButtons() {
  rectMode(CENTER);

  if (buttonLClicked) {
    fill(color(150));
  } else {
    fill(color(200));
  }
  stroke(255);

  rect(rectLX, rectLY, rectW, rectH);
  fill(color(0));
  textSize(40);
  text("<", rectLX-20, rectLY+10);


  if (buttonRClicked) {
    fill(color(150));
  } else {
    fill(color(200));
  }
  rect(rectRX, rectRY, rectW, rectH);
  fill(color(0));
  textSize(40);
  text(">", rectRX-10, rectRY+10);
}


void draw() {    
  clear();
  Update();
  if (backgroundImg != null)
    background(backgroundImg);  
  else
    background(230);  

  if (frames.size()==0) {
    for (int i=0; i<zooidManager.getNbZooids(); i++) {
      fill(zooidManager.getZooidColor(i));
      ellipse(zooidManager.getZooidPosition(i).x, 
        zooidManager.getZooidPosition(i).y, 
        zooidManager.getZooidSize(), 
        zooidManager.getZooidSize());
    }
  } else {
    if (currentIndex>=0) {
      for (int i=0; i<frames.get(currentIndex).length; i++) {
        fill(frames.get(currentIndex)[i].getColour());
        ellipse(frames.get(currentIndex)[i].getPosition().x, 
          frames.get(currentIndex)[i].getPosition().y, 
          zooidManager.getZooidSize(), 
          zooidManager.getZooidSize());
      }
    }
  }
  fill(color(0));
  textSize(24);
  text("Frame: ", 10, 30);

  int space = 95;
  for (int i=0; i<frames.size(); i++) {
    if (i==currentIndex)
      fill(color(200, 0, 0));
    else
      fill(color(0));

    text(str(i), space, 30);
    if (i<10) 
      space+=20;
    else 
    space += 32;
  }

  fill(color(0));
  text("Speed: "+ nf(speed, 1, 2) + "x", 10, 60);
  if (playing)
    text("Playing", 10, 90);
  else
    text("Stop", 10, 90);

  drawButtons();
}

void keyReleased()
{
  if (key == '-' || key == '_') {
    if (timestep < 8000) {
      timestep *= 2;
      speed = 1000.0f/float(timestep);
    }
  } else if (key == '+' || key == '=') {
    if (timestep > 125) {
      timestep /= 2;
      speed = 1000.0f/float(timestep);
    }
  } else if (key == ' ') {
    playing = !playing;
  } else if (keyCode == LEFT) {
    currentIndex = (currentIndex>0) ? (currentIndex-1):frames.size()-1;
  } else if (keyCode == RIGHT) {
    currentIndex = (currentIndex+1)%frames.size(); 
    println(currentIndex);
  } else if (keyCode == ENTER || keyCode == RETURN) {
    if (addFrame()) {
      currentIndex++;
    }
  } else if (keyCode == BACKSPACE || keyCode == DELETE) {
    if (removeFrame(currentIndex)) {
      currentIndex--;
    }
  }
}


void save(File folder) {
  if (folder != null) {
    try {
      PrintWriter saveFile = createWriter(folder+"/positions.zoo");
      JSONObject root = new JSONObject();
      root.setInt("num_frames", frames.size());
      JSONArray frameArray = new JSONArray();

      for (int i=0; i<frames.size(); i++) {

        JSONArray f = new JSONArray();

        for (int j=0; j<frames.get(i).length; j++) {
          JSONObject point = new JSONObject();

          point.setInt("id", frames.get(i)[j].getId());

          JSONArray pos = new JSONArray();
          pos.setFloat(0, frames.get(i)[j].getPosition().x);
          pos.setFloat(1, frames.get(i)[j].getPosition().y);
          point.setJSONArray("position", pos);

          JSONArray col = new JSONArray();
          col.setInt(0, int(red(frames.get(i)[j].getColour())));
          col.setInt(1, int(green(frames.get(i)[j].getColour())));
          col.setInt(2, int(blue(frames.get(i)[j].getColour())));   
          point.setJSONArray("color", col);

          f.setJSONObject(j, point);
        }
        frameArray.setJSONArray(i, f);
      }
      root.setJSONArray("frames", frameArray);

      saveFile.println(root.toString());
      saveFile.flush();
      saveFile.close();
    }
    catch(Exception e) {
    }
  }
  super.exit();
}

void load(File selection) {
  if (selection != null) {
    try {
      byte data[] = loadBytes(selection.getAbsolutePath()); 
      if (data != null) {
        JSONObject savedFile = JSONObject.parse(new String(data));

        if (savedFile == null) {
          println("JSONObject could not be parsed");
        } else {    
          int numFrames = savedFile.getInt("num_frames");

          JSONArray frameArray = savedFile.getJSONArray("frames");

          if (frameArray.size() == numFrames) {
            for (int i = 0; i < frameArray.size(); ++i) {
              JSONArray f = frameArray.getJSONArray(i);
              Point[] points = new Point[f.size()];

              for (int j=0; j<f.size(); ++j) {
                JSONObject point = f.getJSONObject(j);

                JSONArray posArray = new JSONArray();
                PVector pos = new PVector();
                JSONArray colArray = new JSONArray();
                color col = color(0);
                int id = -1;
                if (point.hasKey("id"))
                  id = point.getInt("id");
                if (point.hasKey("position")) {
                  posArray = point.getJSONArray("position");
                  pos = new PVector(posArray.getFloat(0), posArray.getFloat(1));
                }
                if (point.hasKey("color")) { 
                  colArray = point.getJSONArray("color");
                  col = color(colArray.getInt(0), colArray.getInt(1), colArray.getInt(2));
                }
                points[j] = new Point(id, pos, col);
              }
              frames.add(points);
              currentIndex++;
            }
          }
        }
      }
    }
    catch(Exception e) {
      println(e);
    }
  }
}

boolean overRect(int x, int y, int width, int height) {
  if (mouseX >= x && mouseX <= x+width && 
    mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

void mousePressed() {
  if (overRect(rectLX-rectW/2, rectLY-rectH/2, rectW, rectH)) {
    buttonLClicked = true;
    currentIndex = (currentIndex>0) ? (currentIndex-1):frames.size()-1;
  } else if (overRect(rectRX-rectW/2, rectRY-rectH/2, rectW, rectH)) {
    buttonRClicked = true;
    currentIndex = (currentIndex+1)%frames.size();
  }
}

void mouseReleased() {
  buttonLClicked = false;
  buttonRClicked = false;
}