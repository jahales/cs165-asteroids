#ifndef _WIN32 // Tell visual studio to ignore this

#include "vector.h"
#include <iostream>
using namespace std;

void prompt(Vector &vector, int num);
void testAdvance(Vector &vector);
void testGetMag(Vector &vector);
void testGetAngle(Vector &vector);
void testDifference(Vector &vector, Vector &vector2);

int main()
{
   Vector vector1;
   Vector vector2;
   
   prompt(vector1, 1);
   testAdvance(vector1);
   testGetMag(vector1);
   testGetAngle(vector1);
   
   prompt(vector2, 2);
   testAdvance(vector2);
   testGetMag(vector2);
   testGetAngle(vector2);

   testDifference(vector1, vector2);
   return 0;
}

void prompt(Vector &vector, int num)
{
   float tempX;
   float tempY;
   float tempDX;
   float tempDY;
   
   cout << "Enter x" << num << ", y" << num << ": ";
   cin >> tempX >> tempY;
   cout << "Enter dx" << num << ", dy" << num << ": ";
   cin >> tempDX >> tempDY;

   vector.setX(tempX);
   vector.setY(tempY);
   vector.setDX(tempDX);
   vector.setDY(tempDY);
}

void testAdvance(Vector &vector)
{
   vector.advance(1.0);

   cout << "New x, y: " << vector.getX()
        << ", " << vector.getY() << endl;
}

void testGetMag(Vector &vector)
{
   cout << "Mag of dx/dy vector: " << vector.getMag() << endl;
}

void testGetAngle(Vector &vector)
{
   cout << "Angle of dx/dy vector: " << endl;
}

void testDifference(Vector &vector, Vector &vector2)
{
   cout << "Difference between the two: "
        << vector - vector2 << endl;
}

#endif
