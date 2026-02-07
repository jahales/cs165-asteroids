#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "environment.h"
#include "vector.h"
#include "point.h"
#include "sprite.h"
#include "graphics.h"
#include <vector>
#include <iostream>
using namespace std;

class Environment;

/******************************************************************************
 * Moveable: basic type of moveable pieces
 *****************************************************************************/
class Moveable
{
public:

   Moveable(Environment* pEnvironment, Vector v);
   virtual ~Moveable(); 

   bool isDead() { return fDead; }
   void setRotation(float rot);
      
   float  getRotation() const { return rotation; }
   Vector getVector() const { return vector;   }

   virtual void kill() { fDead = true; }
   virtual void draw(float dt);
   virtual int  getSize()  const { return size; }
   virtual char getType()  const { return  ' '; }

   virtual void operator+= (float dt) { vector.advance(dt); wrap(); };//advance

   void wrap();

protected:

   Vector       vector;
   float        rotation;
   Environment* pEnvironment;
   int          size;

   std::vector<Sprite*> sprites;

   void addSprite(std::string texture);

private:

   bool         fDead;
};

/******************************************************************************
 * Bullet: the bullet shot by a ship
 *****************************************************************************/
class Bullet : public Moveable
{
public:

   //takes in vector of ship
   Bullet(Environment* pEnvironment, Vector v, float angle); 

   virtual char getType() const { return 'b'; }
   virtual int  getSize() const { return   1; }
   virtual void operator += (float dt); //advance

protected:

   float timeLeft;

};

/******************************************************************************
 * Missile: the bullet shot by an enemy ship
 *****************************************************************************/
class Missile : public Bullet
{
public:
   //takes in vector of enemy ship
   Missile(Environment* pEnvironment, Vector v, float angle); 

   virtual char getType() const { return 'm'; }
};

/******************************************************************************
 * Explosion:
 *****************************************************************************/
class Explosion : public Moveable
{
protected:

   float mElapsed;

public:

   Explosion(Environment* pEnvironment, Vector v, std::string texture);

   virtual char getType() const { return ' '; }
   virtual void operator += (float dt);
};

/******************************************************************************
 * Shootable: Objects that can be 'killed' by bullet
 *****************************************************************************/
class Shootable : public Moveable
{
public:

   Shootable(Environment* pEnvironment, Vector v) 
      : Moveable(pEnvironment, v) { }

private:

};

/******************************************************************************
 * Ship: spaceship that moves around screen
 *****************************************************************************/
class Ship : public Shootable
{
public:
   Ship(Environment* pEnvironment, Vector v);
   ~Ship();

   virtual void operator += (float dt); //advance
   virtual char getType() const { return 's'; }
   virtual int  getSize() const {   return 1; }

protected:
   float cooldown;
private:
   bool hasThrust;

   void setHasThrust(bool isThrusting);   
};

/******************************************************************************
 * Saucer: an enemy ship that attempts to shoot you.
 ******************************************************************************/
class Saucer : public Ship
{
public:
	Saucer(Environment* pEnvironment, Vector v);
	~Saucer();
	
	virtual void operator += (float dt); //advance
	virtual char getType() const { return 'e'; }
	virtual int  getSize() const { return  10; }
};

/******************************************************************************
 * Rock: base class for an asteroid
 *****************************************************************************/
class Rock : public Shootable
{
public:

   Rock(Environment* pEnvironment, Vector v);

   virtual char getType() const { return 'r'; }

   virtual void operator+=(float dt);

protected:
	float dRotation; //rotation speed
};

/******************************************************************************
 * LargeRock: largest asteroid
 *****************************************************************************/
class LargeRock : public Rock
{
public:

   LargeRock(Environment* pEnvironment, Vector v);
   ~LargeRock();

private:

};

/******************************************************************************
 * MedRock: medium sized asteroid
 *****************************************************************************/
class MedRock : public Rock
{
public:

   MedRock(Environment* pEnvironment, Vector v);
   ~MedRock();

private:

};

/******************************************************************************
 * SmallRock: smallest asteroid
 *****************************************************************************/
class SmallRock : public Rock
{
public:

   SmallRock(Environment* pEnvironment, Vector v);
   ~SmallRock();

private:

};

#endif
