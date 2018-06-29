public class Point
{
  private int id;  
  private PVector position;
  private color colour;

  public Point()
  {
    this.id = 0;
    this.position = new PVector(0.0f, 0.0f);
    this.colour = color(0);
  }

  public Point(int _id, PVector _position, color _colour) 
  {
    this.id = _id;
    
    this.position = new PVector(_position.x, _position.y);
    this.colour = _colour;
  }

  public void setId(int _id)
  {
    this.id = _id;
  }

  public void setPosition(PVector _position)
  {
    this.position = _position;
  }

  public void setColour(color _colour)
  {
    this.colour = _colour;
  }

  public int getId() 
  {
    return this.id;
  }

  public PVector getPosition()
  {
    return this.position;
  }

  public color getColour()
  {
    return this.colour;
  }
}