/******************************************************************************
 * environment.h: defines the environment class which is responsible for 
 *    maintaining the state of the game
 *****************************************************************************/
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "graphics.h"
#include "sprite.h"
#include "audio.h"
#include "entity.h"
#include "vector.h"
#include <list>

/******************************************************************************
 * Macros for Sprites (SPR) and WAV files (WAV)
 *****************************************************************************/
#ifdef _WIN32
   #define SPR(name) (std::string("images/") + (name) + ".spr")
   #define WAV(name) (std::string("sound/")  + (name) + ".wav")
#else
   #define SPR(name) (std::string("./images/") + (name) + ".spr")
   #define WAV(name) (std::string("./sound/")  + (name) + ".wav")
#endif

/******************************************************************************
 * Forward declarations
 *****************************************************************************/
class Moveable;

/******************************************************************************
 * environment: this class is responsible for maintaining the state of the 
 *    game such as the graphics, audio, and game entities
 *****************************************************************************/
class Environment : public IGraphicsCallback
{
private:

   Sprite*              mpBackground;
   Sprite*              mpMenu;
   Sprite*              mpTopMenu;
   Sprite*              mpGameOver;
   Moveable*            mpShip;
   Graphics             mGraphics;
	AudioManager         mAudioManager;
   std::list<Moveable*> mEntities;
   bool                 mKeyStates[SDL_NUM_SCANCODES];
   int                  mAsteroidCount;
   int						mGameScore;
	int						mWaveNumber;
	bool						mSaucerAttack;
   int                  mLivesRemaining;
   float                mMenuCountdown;
   float                mShipCountdown;
   bool                 mPaused;
   bool                 mGameOver;

   /***************************************************************************
    * renderScene: causes the environment to render each of the stored game 
    *    entities and draws other graphical elements
    *    INPUT: dt: number of milliseconds elapsed since last render
    **************************************************************************/
   virtual void renderScene(float dt);

   /***************************************************************************
    * keyUp: triggered when a key is released
    *    INPUT: key: the ascii character value
    **************************************************************************/
   virtual void keyUp(SDL_Keycode key);

   /***************************************************************************
    * keyDown: triggered when a key is pressed
    *    INPUT: key: the ascii character value
    **************************************************************************/
   virtual void keyDown(SDL_Keycode key);

public:

   Environment(int argc, char* argv[]);
   ~Environment();

   /***************************************************************************
    * Getters
    **************************************************************************/
   float getXMax() const { return  mGraphics.getWidth();   }
   float getXMin() const { return                     0;   }
   float getYMax() const { return mGraphics.getHeight();   }
   float getYMin() const { return                     0;   }

   Graphics*     getGraphics() { return &mGraphics;     }
   AudioManager* getAudio()    { return &mAudioManager; }

   /***************************************************************************
   * Setters
   ****************************************************************************/
   void addPoint() { mGameScore++; };
	void addPoint(int num) { mGameScore += num; };
   void addRockNum(int num) { mAsteroidCount += num; };

   /***************************************************************************
    * Key state checks
    **************************************************************************/
   bool isSpace() const { return mKeyStates[SDL_SCANCODE_SPACE]; }
   bool isLeft () const { return mKeyStates[SDL_SCANCODE_LEFT ]; }
   bool isRight() const { return mKeyStates[SDL_SCANCODE_RIGHT]; }
   bool isUp   () const { return mKeyStates[SDL_SCANCODE_UP   ]; }
   bool isDown () const { return mKeyStates[SDL_SCANCODE_DOWN ]; }
   bool isKey  (char c) const { return mKeyStates[SDL_GetScancodeFromKey(c)]; }

   /***************************************************************************
    * start: starts the environment by running the message-processing loop
    **************************************************************************/
   void start();

   /***************************************************************************
    * add: adds a new entity to the environment
    *    INPUT: pEntity: pointer to the entity
    **************************************************************************/
   void add(Moveable* pEntity);

   void nextWave();

	void saucerAttack();

   void addShip(bool subtractLife);

   void collide(Moveable * &m1, Moveable * &m2);
};

#endif