/******************************************************************************
 * environment.cpp: implements the Environment class
 *****************************************************************************/
#include "environment.h"
#include <math.h>
#include <iostream>
using namespace std;

Environment::Environment(int argc, char* argv[])
   : mGraphics(400, 400, "Asteroids!"), mAsteroidCount(0), mGameScore(0),
	mWaveNumber(0)
{
   memset(mKeyStates, false, sizeof(mKeyStates));

   mMenuCountdown  = 3.0;
   mShipCountdown  = 3.0;
   mLivesRemaining = 3;
   mPaused         = false;
   mGameOver       = false;
	mSaucerAttack	 = false;

   mpBackground = new TilingSprite(&mGraphics, SPR("stars"),     400, 400);
   mpMenu       = new       Sprite(&mGraphics, SPR("menu"),      320, 240);
   mpGameOver   = new       Sprite(&mGraphics, SPR("game-over"), 320, 240);
   mpTopMenu    = new       Sprite(&mGraphics, SPR("top-menu"),   400, 24);

   addShip(false);

   mAudioManager.play(WAV("rachmaninov"), true);
}

Environment::~Environment()
{
   delete mpBackground;
   delete mpMenu;
   delete mpGameOver;
   delete mpTopMenu; 

   for (list<Moveable*>::iterator it = mEntities.begin();
      it != mEntities.end();
      it++)
   {
      delete *it;
   }
}

void Environment::addShip(bool subtractLife)
{
   if (mGameOver)
   {
      return;
   }
   if (subtractLife)
   {
      mLivesRemaining--;
   }
   if (mLivesRemaining >= 0)
   {
      mpShip = new Ship(this, Vector(getXMax() / 2, getYMax() / 2, 0, 0));
      add(mpShip);
      mShipCountdown = 3.0;
   }
   else
   {
      mGameOver = true;
      mLivesRemaining = 0; 
   }
}

void Environment::nextWave()
{
   // Was the ship destroyed?
   if (mGameOver || mpShip == NULL)
      return;

   // The game will become progressively more difficult
   float delta = log((float)mGameScore + 1.0) / 2;

   // Create the asteroids
   Vector center = mpShip->getVector();
   mAsteroidCount = 5 + delta;
   
   for (int i = 0; i < mAsteroidCount; i++)
   {
      // Set the position away from where the ship is
      Vector v;

      do 
      {
         v.setX( Graphics::random(getXMin(), getXMax()));
         v.setY( Graphics::random(getYMin(), getYMax()));
         v.setDX(Graphics::random(-10 - delta, 10 + delta) * (1 + delta));
         v.setDY(Graphics::random(-10 - delta, 10 + delta) * (1 + delta));
      }
      while ((center - v) < (100 - delta));

      // Generate some new asteroids
      switch (Graphics::random(1, 3))
      {
      case 1:
         add(new SmallRock(this, v));
         break;
      case 2:
         add(new   MedRock(this, v));
         break;
      case 3:
         add(new LargeRock(this, v));
         break;
      }
   }
}

/******************************************************************************
 * A spaceship comes onto the screen and fires randomly aimed bullets.
 ******************************************************************************/
void Environment::saucerAttack()
{
	mSaucerAttack = true;

	// Was the ship destroyed?
   if (mGameOver || mpShip == NULL)
      return;

	//initialize vector at middle, right side of screen
	Vector v(getXMax(), (getYMax() - getYMin()) / 2, -20, 0);

	//create the enemy ship
	add(new Saucer(this, v));


}

/******************************************************************************
 * start: starts the environment by running the message-processing loop
 *****************************************************************************/
void Environment::start()
{
   mGraphics.run(this);
}

/******************************************************************************
 * keyUp: triggered when a key is released
 *    INPUT: key: the ascii character value
 *****************************************************************************/
void Environment::keyUp(SDL_Keycode key)
{ 
   SDL_Scancode sc = SDL_GetScancodeFromKey(key);
   if (sc < SDL_NUM_SCANCODES)
      mKeyStates[sc] = false;
}

/******************************************************************************
 * keyDown: triggered when a key is pressed
 *    INPUT: key: the ascii character value
 *****************************************************************************/
void Environment::keyDown(SDL_Keycode key) 
{
   SDL_Scancode sc = SDL_GetScancodeFromKey(key);
   if (sc < SDL_NUM_SCANCODES)
      mKeyStates[sc] = true;

   switch (key)
   {
   case 'm':
      mMenuCountdown = (mMenuCountdown > 0) ? 0 : 5;
      break;
   case 'p':
      mPaused = !mPaused;
      break;
   }
}

/***************************************************************************
 * add: adds a new entity to the environment
 *    INPUT: pEntity: pointer to the entity
 **************************************************************************/
void Environment::add(Moveable* pEntity)
{
   mEntities.push_back(pEntity);
}

/******************************************************************************
 * renderScene: causes the environment to render each of the stored game 
 *    entities
 *    INPUT: dt: number of milliseconds elapsed since last render
 *****************************************************************************/
void Environment::renderScene(float dt)
{
   // Check current status
   if (!mGameOver && mpShip == NULL)
   {
      if (mShipCountdown < 0)
         addShip(true);
      mShipCountdown -= dt;
   }

   // Draw the background (bottommost layer)
   mpBackground->draw(getXMax() / 2, getYMax() / 2, 0, dt);

   // Update each object
   for (list<Moveable*>::iterator it = mEntities.begin();
      it != mEntities.end();)
   {
      Moveable* p = (*it);

      // Only advance when we're not paused
      if (!mPaused && mMenuCountdown <= 0)
	      *p += dt; //advance
      p->draw(dt);

      // detect collisions
      for (list<Moveable*>::iterator it2 = mEntities.begin();
         it2 != mEntities.end();
         it2++)
      {
         Moveable* p2 = (*it2);

         if (p != p2 && p->getVector() - p2->getVector() 
            <= (float)(p->getSize() + p2->getSize()))
         {
            collide(p, p2);
         }
      }

      // delete dead objects
      if (p->isDead())
      {
         it = mEntities.erase(it);
         delete p;
      }
      else
      {
         it++;
      }

		if (!mSaucerAttack)
		{
			// Add new asteroids?
			if (mAsteroidCount == 0 && mpShip)
			{
				if (mWaveNumber % 2 == 1) //every other wave
				{
					saucerAttack(); //saucer counts as a wave
				}
				else
				{
					nextWave();
				}

				mWaveNumber++;
			}
		}
   }

   // Draw top-level menu items
   mpTopMenu->draw(getXMax() / 2, getYMax() - 12, 0, dt);

   mGraphics.drawNumber(getXMin() + 125, getYMax() - 7, mGameScore, false);
   mGraphics.drawNumber(getXMax() - 15, getYMax() - 7, mLivesRemaining, false);

   if (!mGameOver && mMenuCountdown > 0)
   {
      mpMenu->draw(getXMax() / 2, getYMax() / 2, 0, dt);
      mMenuCountdown -= dt;
   }
   else if (mGameOver)
   {
      mpGameOver->draw(getXMax() / 2, getYMax() / 2, 0, dt);
   }
}

void Environment::collide(Moveable * &m1, Moveable * &m2)
{
   switch(m1->getType())
   {
      case 'b': //bullet
         if (m2->getType() == 'r' &&
             !(m1->isDead())) //check that bullet is unused (alive)
         {
            m1->kill(); //kill bullet
            m2->kill(); //kill asteroid
            mAsteroidCount--;
         }
			else if (m2->getType() == 'e' && !(m2->isDead()))
         {
            m1->kill(); //kill bullet
            m2->kill(); //kill saucer
            mSaucerAttack = false;
				addPoint(10);
         }
         break;
      case 's': //ship
         if (m2->getType() == 'r' && !(m1->isDead()))
         {
            m1->kill();    //kill ship
            m2->kill();    //kill asteroid
            mpShip = NULL; //clear ship pointer
            mAsteroidCount--;
         }
			else if (m2->getType() == 'm' && !(m2->isDead()))
         {
            m1->kill();    //kill ship
            m2->kill();    //kill missile
            mpShip = NULL; //clear ship pointer
         }
			else if (m2->getType() == 'e' && !(m2->isDead()))
         {
            m1->kill();    // kill ship
            m2->kill();    // kill saucer
            mpShip = NULL; //clear ship pointer
            mSaucerAttack = false;
				addPoint(10);
         }
         break;
      case 'r': //rock
         if (m2->getType() == 'b' && !(m2->isDead()))
         {
            m1->kill(); //kill asteroid
            m2->kill(); //kill bullet
            mAsteroidCount--;
         }
         else if (m2->getType() == 's' && !(m2->isDead()))
         {
            m1->kill();    // kill asteroid
            m2->kill();    // kill ship
            mpShip = NULL; //clear ship pointer
            mAsteroidCount--;
         }
         break;
		case 'e': //enemy alien ship - immune to asteroids
			if (m2->getType() == 'b' && !(m2->isDead()))
         {
            m1->kill(); //kill saucer
            m2->kill(); //kill bullet
            mSaucerAttack = false;
				addPoint(10);
         }
         else if (m2->getType() == 's' && !(m2->isDead()))
         {
            m1->kill();    // kill saucer
            m2->kill();    // kill ship
            mpShip = NULL; //clear ship pointer
            mSaucerAttack = false;
				addPoint(10);
         }
			break;
      default:
         break;
   }
}