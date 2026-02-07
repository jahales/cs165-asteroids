#include "entity.h"
#include <iostream>
using namespace std;

/******************************************************************************
 * Moveable
 *****************************************************************************/
Moveable::Moveable(Environment* pEnvironment, Vector v)
   : fDead(false), rotation(0), pEnvironment(pEnvironment), vector(v)
{
}

Moveable::~Moveable()
{
   for (std::vector<Sprite*>::iterator it = sprites.begin();
      it != sprites.end();
      it++)
   {
      delete *it;
   }
}

void Moveable::setRotation(float rot)
{
   rotation = rot;
   if (rotation >= 360)
      rotation -= 360; //reset to 0 degrees
   else if (rotation < 0)
      rotation += 360; //reset to 360 degrees
}

void Moveable::addSprite(string texture)
{
   sprites.push_back(new AnimatedSprite(
      pEnvironment->getGraphics(), texture, size * 2, size * 2));
}

void Moveable::draw(float dt)
{
   for (std::vector<Sprite*>::iterator it = sprites.begin();
      it != sprites.end();
      it++)
   {
      (*it)->draw(vector.getX(), vector.getY(), getRotation(), dt);
   }
}

void Moveable::wrap()
{
   if (vector.getX() > pEnvironment->getXMax()) //right
      vector.setX(pEnvironment->getXMin());
   if (vector.getX() < pEnvironment->getXMin()) //left
      vector.setX(pEnvironment->getXMax());
   if (vector.getY() > pEnvironment->getYMax()) //top
      vector.setY(pEnvironment->getYMin());
   if (vector.getY() < pEnvironment->getYMin()) //bottom
      vector.setY(pEnvironment->getYMax());
}

/******************************************************************************
 * Bullet
 *****************************************************************************/
Bullet::Bullet(Environment* pEnvironment, Vector v, float angle) 
   : Moveable(pEnvironment, v)
{  
	//play a 'fire bullet' sound
	pEnvironment->getAudio()->play(WAV("fire"), 0);

   size = 5;
   addSprite(SPR("bullet"));

   //add in the direction ship is facing
   // (use minus sign because object rotation is reversed)
   vector.setDX((float)(v.getDX()
		- calcDX((float)(angle + 275), 300)));
   vector.setDY((float)(v.getDY()
		- calcDY((float)(angle + 275), 300)));
   
   //copy position of ship
   vector.setX(v.getX());
   vector.setY(v.getY());

   timeLeft = .85;
}

void Bullet::operator += (float dt) //advance
{ 
   vector.advance(dt);
   wrap(); 
   timeLeft -= dt;

   if (timeLeft <= 0)
      kill();
}

/******************************************************************************
 * Missile: The bullet fired by an enemy ship
 *****************************************************************************/
Missile::Missile(Environment* pEnvironment, Vector v, float angle) 
   : Bullet(pEnvironment, v, angle)
{  
	//play a 'fire bullet' sound
	//pEnvironment->getAudio()->play(WAV("fire"), 0);

   size = 5;
   addSprite(SPR("bullet"));

   //add in the direction of angle
   // (use minus sign because object rotation is reversed)
   vector.setDX((float)(v.getDX()
		- calcDX((float)(angle + 275), 300)));
   vector.setDY((float)(v.getDY()
		- calcDY((float)(angle + 275), 300)));
   
   //copy position of ship
   vector.setX(v.getX());
   vector.setY(v.getY());

   timeLeft = .8;
}

/******************************************************************************
 * Rock: The base class for asteroids
 *****************************************************************************/
Rock::Rock(Environment* pEnvironment, Vector v) : Shootable(pEnvironment, v)
{
	rotation = (float)((rand() % 36) * 10); //random starting rotation
	dRotation = (float)(rand() % 2); //one or zero
	if (dRotation == 0)
		dRotation = -1;
}

void Rock::operator+=(float dt)
{
   vector.advance(dt);
   rotation += dRotation * dt; 
   wrap();
}

/******************************************************************************
 * SmallRock
 *****************************************************************************/
SmallRock::SmallRock(Environment* pEnvironment, Vector v)
   : Rock(pEnvironment, v)
{
   size = 15;
   addSprite(SPR("asteroid"));
   dRotation *= 200; // + or - 1 times 300
}

SmallRock::~SmallRock()
{
	//play an 'explosion' sound
	pEnvironment->getAudio()->play(WAV("bangSmall"), 0);

	//add point to the game score
	pEnvironment->addPoint();
}

/******************************************************************************
 * MedRock
 *****************************************************************************/
MedRock::MedRock(Environment* pEnvironment, Vector v)
   : Rock(pEnvironment, v)
{
   size = 20;
   addSprite(SPR("asteroid"));
   dRotation *= 75; // + or - 1 times 150
}

MedRock::~MedRock()
{
	//play an 'explosion' sound
	pEnvironment->getAudio()->play(WAV("bangMedium"), 0);

	//add point to the game score
	pEnvironment->addPoint();

	//add 2 to mAsteroidCount
	pEnvironment->addRockNum(2);

	//make a temporary copy of destroyed rock's vector
	Vector oldVector(this->getVector());

	//add new asteroids where medium one died
	Vector temp(0, 0, 30, 0); //x, y, dx, dy --> dx goes right by 3
	oldVector += temp;
	pEnvironment->add(new SmallRock(pEnvironment, oldVector)); //small rock

	temp.setDX(-60); //reset dx and move dx left by 3
	oldVector += temp;
   	pEnvironment->add(new SmallRock(pEnvironment, oldVector)); //small rock
}

/******************************************************************************
 * LargeRock
 *****************************************************************************/
LargeRock::LargeRock(Environment* pEnvironment, Vector v)
   : Rock(pEnvironment, v)
{
   size = 25;
   addSprite(SPR("asteroid"));

   dRotation *= 50; // + or - 1 times 100
}

//Large Rock destructor: creates 3 new asteroids of smaller sizes
LargeRock::~LargeRock()
{
	//play an 'explosion' sound
	pEnvironment->getAudio()->play(WAV("bangLarge"), 0);

	//add point to the game score
	pEnvironment->addPoint();

	//add 3 to mAsteroidCount
	pEnvironment->addRockNum(3);

	//make a temporary copy of destroyed rock's vector
	Vector oldVector(this->getVector());

	//add new asteroids where large one died
	Vector temp(0, 0, 0, 10); //x, y, dx, dy --> dy goes up 1
	oldVector += temp;
	pEnvironment->add(new MedRock(pEnvironment, oldVector)); //medium rock
	
	temp.setDY(-20); // dy goes down 1
	oldVector += temp;
	pEnvironment->add(new MedRock(pEnvironment, oldVector)); //medium rock

	temp.setDY(10); //reset dy to 0
	temp.setDX(20); //dx goes right by 2
	oldVector += temp;
	pEnvironment->add(new SmallRock(pEnvironment, oldVector)); //small rock
}

/******************************************************************************
 * Ship
 *****************************************************************************/
Ship::Ship(Environment* pEnvironment, Vector v)
   : Shootable(pEnvironment, v), cooldown(0)
{
   hasThrust = false;
   size = 16;
   addSprite(SPR("ship-norm"));
   addSprite(SPR("ship-thrust"));
   sprites[1]->setVisible(false);
}

Ship::~Ship()
{
   // Make sure audio is cleared out
   setHasThrust(false);
   pEnvironment->add(new Explosion(pEnvironment, vector, SPR("explosion-orange")));
}

void Ship::setHasThrust(bool isThrusting)
{
   if (hasThrust && !isThrusting)
   {
      sprites[0]->setVisible(true);
      sprites[1]->setVisible(false);
      pEnvironment->getAudio()->stop(WAV("thrust"));
   }
   else if (!hasThrust && isThrusting)
   {
      sprites[0]->setVisible(false);
      sprites[1]->setVisible(true);
      pEnvironment->getAudio()->play(WAV("thrust"), true);
   }
   hasThrust = isThrusting;
}

void Ship::operator +=(float dt) //advance
{
   vector.advance(dt);
   cooldown -= dt;
   wrap();

   // Check keyboard
   if (pEnvironment->isSpace() && cooldown < 0)
   {
      cooldown = 0.5;
      pEnvironment->add(new Bullet(pEnvironment, vector, getRotation()));
   }
   
   // Check for movement
   setHasThrust(pEnvironment->isUp()); 

   // Process movement based on direction
   if (pEnvironment->isLeft())
   {
      setRotation(getRotation() + 100 * dt);
   }
   if (pEnvironment->isRight())
   {
      setRotation(getRotation() - 100 * dt);
   }  
   if (pEnvironment->isUp())
   {
      vector.setDX((float)(getVector().getDX()
			- calcDX((float)(getRotation() + 275), 50 * dt)));
      vector.setDY((float)(getVector().getDY()
			- calcDY((float)(getRotation() + 275), 50 * dt)));
   }
   if (pEnvironment->isDown())
   {
      vector.setDX((float)(getVector().getDX() / (1 + dt * 1.1)));
      vector.setDY((float)(getVector().getDY() / (1 + dt * 1.1)));
   }
   if (pEnvironment->isKey('h') && cooldown <= 0)
   {
      float x = Graphics::random(
         pEnvironment->getXMin(), pEnvironment->getXMax());
      float y = Graphics::random(
         pEnvironment->getYMin(), pEnvironment->getYMax());

      pEnvironment->add(new Explosion(pEnvironment, vector, SPR("explosion-blue")));
      pEnvironment->getAudio()->play(WAV("extraShip"));

      vector.setX(x);
      vector.setY(y);

      cooldown = 1;
   }
}

/******************************************************************************
 * Saucer: Initialize movement by changing vector before creating.
 *****************************************************************************/
Saucer::Saucer(Environment* pEnvironment, Vector v)
   : Ship(pEnvironment, v)
{
   size = 16;
   addSprite(SPR("saucer"));

	//start sound loop
	pEnvironment->getAudio()->play(WAV("saucerBig"), true);
}

Saucer::~Saucer()
{
	//end sound loop
	pEnvironment->getAudio()->stop(WAV("saucerBig"));
}

void Saucer::operator +=(float dt) //advance
{
   vector.advance(dt);
   cooldown -= dt;
   wrap();

   // fire random missiles
   if (cooldown < 0)
   {
		float angle = (rand() % 10) * 36;
      cooldown = 0.8;
      pEnvironment->add(new Missile(pEnvironment, vector, angle));
   }
}

/******************************************************************************
 * Explosion:
 *****************************************************************************/
Explosion::Explosion(Environment* pEnvironment, Vector v, string texture)
   : Moveable(pEnvironment, v)
{
   size = 16;
   addSprite(texture);
   mElapsed = 0;
}

void Explosion::operator+=(float dt)
{
   if (mElapsed == 0)
   {
      sprites[0]->changeSize(200, 200, .25);
   }
   else if (!sprites[0]->hasEffects())
   {
      if (mElapsed > .5 && mElapsed < .75)
      {
         sprites[0]->changeSize(32, 32, .25);
      }
      else if (mElapsed > .75)
      {
         kill();
      }
   }

   mElapsed += dt;
}