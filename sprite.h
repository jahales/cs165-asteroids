/******************************************************************************
 * sprite.h: defines the Sprite class and derivatives which are used for 
 *    animating 2D textures in OpenGL. Also includes the Effect classes
 *****************************************************************************/
#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <list>
#include "graphics.h"
#include "texture.h"

/******************************************************************************
 * Forward declarations
 *****************************************************************************/
class Sprite;

/******************************************************************************
 * Effect: animates a sprite with an effect
 *****************************************************************************/
class Effect
{
protected:

   float   mDuration;
   float   mTimeElapsed;
   bool    mIsFinished;
   Sprite* mpSprite;

   /***************************************************************************
   * progress: percentage representing stage of the animation
   *    OUTPUT: <return> returns the percent completion
   **************************************************************************/
   float getProgress() const { return mTimeElapsed / mDuration; }

public:

   /***************************************************************************
   * Effect:
   *    INPUT: pSprite : pointer to the sprite to animate
   *           duration: how long the animation should last in milliseconds 
   **************************************************************************/
   Effect(Sprite* pSprite, float duration = 1);

   /***************************************************************************
   * isFinished: returns true if the animation has finished
   **************************************************************************/
   bool isFinished() const { return mIsFinished; }

   /***************************************************************************
   * operator+=: advances the animation
   *    INPUT: dt: the amount of time that has passed, in seconds
   **************************************************************************/
   virtual void operator+=(float dt) = 0;
};

/******************************************************************************
 * SizeEffect: animates the sprite changing size
 *****************************************************************************/
class SizeEffect : public Effect
{
private:

   float mWidth;
   float mHeight;
   float mNewWidth;
   float mNewHeight;

public:

   /***************************************************************************
   * SizeEffect:
   *    INPUT: pSprite : pointer to the sprite to animate
   *           width   : new width of the sprite
   *           height  : new height of the sprite
   *           duration: how long the animation should last in seconds 
   **************************************************************************/
   SizeEffect(Sprite* pSprite, float width, float height, float duration);

   /***************************************************************************
   * operator+=: advances the animation
   *    INPUT: dt: the amount of time that has passed, in seconds
   **************************************************************************/
   virtual void operator+=(float dt);
};

/******************************************************************************
 * BlinkEffect: animates the sprite blinking
 *****************************************************************************/
class BlinkEffect : public Effect
{
private:

   float mBlinkRate;
   float mLastBlink;

public:

   /***************************************************************************
   * BlinkEffect:
   *    INPUT: pSprite  : pointer to the sprite to animate
   *           blinkRate: how fast the sprite should blink
   *           duration : how long the animation should last in seconds 
   **************************************************************************/
   BlinkEffect(Sprite* pSprite, float blinkRate, float duration);

   /***************************************************************************
   * operator+=: advances the animation
   *    INPUT: dt: the amount of time that has passed, in seconds
   **************************************************************************/
   virtual void operator+=(float dt);
};

/******************************************************************************
 * Sprite: draws 2D textures on the screen using OpenGL
 *****************************************************************************/
class Sprite
{
protected:

   int            mHeight;
   int            mWidth;
   bool           mVisible;
   const Texture* mpTexture;

   std::list<Effect*> mEffects;

   /***************************************************************************
    * render: draws the sprite with the given center, rotation, and texture
    *    coordinates (which determines which part of the image is displayed)
    *    INPUT: x   : center point x position
    *           y   : center point y position
    *           rot : the sprite's rotation
    *           tmin: texture start position (as a percent in decimal form)
    *           tmax: texture end   position (as a percent in decimal form)
    *           dt  : the amount of time that has passed
    **************************************************************************/
   void render(float x, float y, float rot, float tmin, float tmax, float dt);

   /***************************************************************************
    * effects: process all the effects for the sprite
    *    INPUT: dt: amount of time that as elapsed
    **************************************************************************/
   void effects(float dt);

public:

   /***************************************************************************
    * Sprite:
    *    INPUT: pGraphics: reference to the graphics object
    *           texture  : filename of the texture to use
    **************************************************************************/
   Sprite(Graphics* pGraphics, std::string texture);

   /***************************************************************************
    * Sprite:
    *    INPUT: pGraphics: reference to the graphics object
    *           texture  : filename of the texture to use
    *           width    : width  of the sprite (in OpenGL coordinates)
    *           height   : height of the sprite (in OpenGL coordinates)
    **************************************************************************/
   Sprite(Graphics* pGraphics, std::string texture, int width, int height);

   /***************************************************************************
    * ~Sprite:
    **************************************************************************/
   virtual ~Sprite();

   /***************************************************************************
    * Getters
    **************************************************************************/
   int getWidth()    const { return mWidth;   }
   int getHeight()   const { return mHeight;  }
   int isVisible()   const { return mVisible; }
   bool hasEffects() const { return mEffects.size() > 0; }

   /***************************************************************************
    * Setters
    **************************************************************************/
   void setWidth (float width)   { mWidth  = width;    }
   void setHeight(float height)  { mHeight = height;   }
   void setVisible(bool visible) { mVisible = visible; }

   /***************************************************************************
    * draw: draws the sprite to the screen with the given position and rotation
    *    INPUT: x  : center point x position
    *           y  : center point y position
    *           rot: the sprite's rotation
    *           dt : time elapsed since last draw (in milliseconds)
    **************************************************************************/
   virtual void draw(float x, float y, float rot = 0.0, float dt = 0.0);

   /***************************************************************************
    * changeSize: changes the size of the sprite
    *    INPUT: width   : new width of the sprite
    *           height  : new height of the sprite
    *           duration: amount of time to animate (in seconds)
    **************************************************************************/
   void changeSize(float width, float height, float duration);

   /***************************************************************************
    * blink: causes the entity to blink
    *    INPUT: interval: time between blinks
    *           duration: amount of time to animate (in seconds)
    **************************************************************************/
   void blink(float interval, float duration);
};

/******************************************************************************
 * Sprite: draws animated 2D textures on the screen using OpenGL
 *****************************************************************************/
class AnimatedSprite : public Sprite
{
protected:

   int   mFrameRate;
   int   mFrameCount;
   long  mTimeElapsed;

public:

   /***************************************************************************
    * AnimatedSprite:
    *    INPUT: graphics: reference to the graphics object
    *           texture : filename of the texture to use
    **************************************************************************/
   AnimatedSprite(
      Graphics* pGraphic, std::string texture, float frameRate = 10);

   /***************************************************************************
    * AnimatedSprite:
    *    INPUT: graphics: reference to the graphics object
    *           texture : filename of the texture to use
    *           width   : width  of the sprite (in OpenGL coordinates)
    *           height  : height of the sprite (in OpenGL coordinates)
    **************************************************************************/
   AnimatedSprite(Graphics* pGraphic, std::string texture,
      int width, int height, float frameRate = 10);

   /***************************************************************************
    * draw: draws the sprite to the screen with the given position and rotation
    *    INPUT: x  : center point x position
    *           y  : center point y position
    *           rot: the sprite's rotation
    *           dt : time elapsed since last draw (in milliseconds)
    **************************************************************************/
   virtual void draw(float x, float y, float r, float dt);
};

/******************************************************************************
 * Sprite: draws tiling 2D textures on the screen using OpenGL
 *****************************************************************************/
class TilingSprite : public Sprite
{
public:

   /***************************************************************************
    * TilingSprite:
    *    INPUT: graphics: reference to the graphics object
    *           texture : filename of the texture to use
    *           width   : width  of the sprite (in OpenGL coordinates)
    *           height  : height of the sprite (in OpenGL coordinates)
    **************************************************************************/
   TilingSprite(
      Graphics* pGraphic, std::string texture, float width, float height);

   /***************************************************************************
    * draw: draws the sprite to the screen with the given position and rotation
    *    INPUT: x  : center point x position
    *           y  : center point y position
    *           rot: the sprite's rotation
    *           dt : time elapsed since last draw (in milliseconds)
    **************************************************************************/
   virtual void draw(float x, float y, float r = 0.0, float dt = 0.0);
};

#endif