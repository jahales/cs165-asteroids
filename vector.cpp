/**********************************************************
* Program:
*      VECTOR_CPP
*               These functions perform math of velocity
*               and allow the objects to advance along their
*               current path.
**********************************************************/

#include "vector.h"
#include <iostream>
#include <math.h>
using std::cout;

/*******************************************************
 * VECTOR: copy constructor
 *******************************************************/
Vector::Vector(const Vector& rhs) : Point(rhs.x, rhs.y)
{
   dx = rhs.dx;
   dy = rhs.dy;
}

/*********************************************************
 * VECTOR: default constructor
 *********************************************************/
Vector::Vector() : Point(0.0, 0.0)
{
   dx = 0.0;
   dy = 0.0;
}

/*******************************************************
 * VECTOR: initializing constructor
 *******************************************************/
Vector::Vector(float x, float y, float dx, float dy) : Point(x, y)
{
   this->dx = dx;
   this->dy = dy;
}

/*******************************************************
 * ADVANCE : Moves x and y along dx and dy
 *******************************************************/
void Vector::advance(float dt)
{
   addX(dx * dt);
   addY(dy * dt);
}

/******************************************************
 * DIFFERENCE : Calculates distance between two objects
 ******************************************************/
float Vector::operator - (const Vector &rhs) const
{
   //d = sqrt((x2-x1)^2 + (y2-y1)^2)
   float y = rhs.getY() - getY();
   float x = rhs.getX() - getX();

   x *= x;
   y *= y;
   
   return (float)(sqrt(x + y));
}

/***************************************************
* Functions to calculate a new dx and dy by knowing
* the rotation angle and a tangent length.
****************************************************/
float calcDX(float angle, float hypotenuse)
{
	float dx;
	float theta = (float)(angle * PI / 180);
	dx = hypotenuse * cos(theta);
	return dx;
}

float calcDY(float angle, float hypotenuse)
{
	float dy;
	float theta = (float)(angle * PI / 180);
	dy = hypotenuse * sin(theta);
	return dy;
}

/****************************************************
* Adds a vector to this current one.
*****************************************************/
void Vector::operator += (Vector & rhs)
{
	dx = rhs.getDX() + dx;
	dy = rhs.getDY() + dy;
}
