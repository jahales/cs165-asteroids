#ifndef VECTOR_H
#define VECTOR_H
#define PI 3.14159

//#include "point.h"
#include "math.h"

/***************************************************
* Functions to calculate a new dx and dy by knowing
* the rotation angle and a tangent length.
****************************************************/
float calcDX(float angle, float hypotenuse);
float calcDY(float angle, float hypotenuse);

/*******************
 * POINT
 *******************/
class Point
{
public:
   // constructors
   Point()                : x(0.0 ),  y(0.0 ) {};
   Point(const Point &pt) : x(pt.x),  y(pt.y) {};
   Point(float x, float y): x(x   ),  y(y  )  {};
      
   // getters
   float getX()    const { return x;     };
   float getY()    const { return y;     };

   // setters
   void setX(float x)    { this->x = x;  };
   void setY(float y)    { this->y = y;  };
   void addX(float dx)   { this->x += dx; };
   void addY(float dy)   { this->y += dy; };

protected:
   float x;
   float y;

};

class Vector : public Point
{
  public:
   Vector(); //default constructor
   Vector(float x, float y, float dx = 1, float dy = 1); //initialize constructor
   Vector(const Vector& rhs);
   float getDX() const { return dx; };
   float getDY() const { return dy; };
   float getMag() const { return sqrt((dx * dx) + (dy * dy)); };
   void setDX(float dx) { this->dx = dx; };
   void setDY(float dy) { this->dy = dy; };
   void advance(float dt); //moves point along dx/dy line
	float operator - (const Vector &rhs) const; //difference()
   void operator += (Vector & rhs); //add a vector to the current one

  private:
   float dx;
   float dy;
};

#endif // VECTOR_H
