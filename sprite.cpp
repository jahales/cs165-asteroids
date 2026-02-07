#include "sprite.h"
#include <math.h>
using namespace std;

/************************************************************************
 * Effect:
 *    INPUT: pSprite : pointer to the sprite to animate
 *           duration: how long the animation should last in ms
 ***********************************************************************/
Effect::Effect(Sprite* pSprite, float duration)
   : mDuration(duration), mTimeElapsed(0), mpSprite(pSprite),
     mIsFinished(false)
{
}

/******************************************************************************
 * SizeEffect:
 *    INPUT: pSprite : pointer to the sprite to animate
 *           width   : new width of the sprite
 *           height  : new height of the sprite
 *           duration: how long the animation should last in seconds 
 *****************************************************************************/
SizeEffect::SizeEffect(
      Sprite* pSprite, float width, float height, float duration = 0)
   : Effect(pSprite, duration)
{
   mWidth     = pSprite->getWidth();
   mHeight    = pSprite->getHeight();
   mNewWidth  = width;
   mNewHeight = height;
}

/******************************************************************************
* operator+=: advances the animation
*    INPUT: dt: the amount of time that has passed, in seconds
*****************************************************************************/
void SizeEffect::operator+=(float dt) 
{
   float width  = mWidth;
   float height = mHeight;

   if (!mIsFinished)
   {
      float progress = getProgress();
      width  += (mNewWidth  - mWidth)  * progress;
      height += (mNewHeight - mHeight) * progress;
      mIsFinished = mTimeElapsed >= mDuration;
   }

   mpSprite->setHeight(height);
   mpSprite->setWidth(width);

   mTimeElapsed += dt;
}

/******************************************************************************
 * BlinkEffect:
 *    INPUT: pSprite  : pointer to the sprite to animate
 *           blinkRate: how fast the sprite should blink
 *           duration : how long the animation should last in seconds 
 *****************************************************************************/
BlinkEffect::BlinkEffect(Sprite* pSprite, float blinkRate, float duration)
   : Effect(pSprite, duration), mBlinkRate(blinkRate), mLastBlink(0)
{
}

/******************************************************************************
 * operator+=: advances the animation
 *    INPUT: dt: the amount of time that has passed, in seconds
 *****************************************************************************/
 void BlinkEffect::operator+=(float dt)
 {
    if (!mIsFinished)
    {
       if (mTimeElapsed - mBlinkRate > mLastBlink)
       {
         mLastBlink = mTimeElapsed;
         mpSprite->setVisible(!mpSprite->isVisible());
       }
       mIsFinished = mTimeElapsed >= mDuration;
    }
    
    if (mIsFinished) // Not an "else" because this could have just been set
    {
       mpSprite->setVisible(true);
    }
    mTimeElapsed += dt;
 }

/******************************************************************************
 * Sprite:
 *    INPUT: graphics: reference to the graphics object
 *           texture : filename of the texture to use
 *****************************************************************************/
Sprite::Sprite(Graphics* pGraphics, string texture)
{
   mpTexture = pGraphics->loadTexture(texture);
   mWidth    = mpTexture->getHeight();
   mHeight   = mpTexture->getHeight();
   mVisible  = true;
}

/******************************************************************************
 * Sprite:
 *    INPUT: graphics: reference to the graphics object
 *           texture : filename of the texture to use
 *           width   : width  of the sprite (in OpenGL coordinates)
 *           height  : height of the sprite (in OpenGL coordinates)
 *****************************************************************************/
Sprite::Sprite(Graphics* pGraphics, string texture, int width, int height)
{
   mpTexture = pGraphics->loadTexture(texture);
   mWidth    = width;
   mHeight   = height;
   mVisible  = true;
}

/***************************************************************************
 * ~Sprite:
 **************************************************************************/
Sprite::~Sprite()
{
   for (list<Effect*>::iterator it = mEffects.begin();
      it != mEffects.end(); )
   {
      delete (*it); // ... yeah, delete it
   }
}

/***************************************************************************
 * changeSize: changes the size of the sprite
 *    INPUT: width   : new width of the sprite
 *           height  : new height of the sprite
 *           duration: amount of time to animate (in seconds)
 **************************************************************************/
void Sprite::changeSize(float width, float height, float duration)
{
   mEffects.push_back(new SizeEffect(this, width, height, duration));
}

/***************************************************************************
 * blink: causes the entity to blink
 *    INPUT: interval: time between blinks
 *           duration: amount of time to animate (in seconds)
 **************************************************************************/
void Sprite::blink(float interval, float duration)
{
   mEffects.push_back(new BlinkEffect(this, interval, duration));
}

/******************************************************************************
 * effects: process all the effects for the sprite
 *    INPUT: dt: amount of time that as elapsed
 *****************************************************************************/
void Sprite::effects(float dt)
{
   for (list<Effect*>::iterator it = mEffects.begin();
      it != mEffects.end(); )
   {
      if ((*it)->isFinished())
      {
         it = mEffects.erase(it);
      }
      else
      {
         (*(*it)) += dt;
         it++;
      }
   }
}

/******************************************************************************
 * render: draws the sprite with the given center, rotation, and texture
 *    coordinates (which determines which part of the image is displayed)
 *    INPUT: x   : center point x position
 *           y   : center point y position
 *           rot : the sprite's rotation
 *           tmin: texture start position (as a percent in decimal form)
 *           tmax: texture end   position (as a percent in decimal form)
 *****************************************************************************/
void Sprite::render(float x, float y, float rot, float tmin, float tmax, float dt) 
{
   effects(dt); // Run the effects before rendering

   if (!mVisible)
      return;

   // Set box around center point (px, py)
   float xmin = x -  mWidth / 2;
   float xmax = x +  mWidth / 2;
   float ymin = y - mHeight / 2;
   float ymax = y + mHeight / 2;

   glPushMatrix();

   glTranslatef(x, y, 0);
   glRotatef(rot, 0.0, 0.0, 1.0);
   glTranslatef(-x, -y, 0);

   // Bind textures and draw a textured rectangle
   glBindTexture(GL_TEXTURE_2D, mpTexture->getId());

   glBegin(GL_QUADS);

   glTexCoord2f(tmin,    1);
   glVertex2f(  xmin, ymin);
 
   glTexCoord2f(tmax,    1);
   glVertex2f(  xmax, ymin);
 
   glTexCoord2f(tmax,    0);
   glVertex2f(  xmax, ymax);
 
   glTexCoord2f(tmin,    0);
   glVertex2f(  xmin, ymax);

   glEnd();

   glPopMatrix();
}

/******************************************************************************
 * draw: draws the sprite to the screen with the given position and rotation
 *    INPUT: x  : center point x position
 *           y  : center point y position
 *           rot: the sprite's rotation
 *           dt : time elapsed since last draw (in milliseconds)
 *****************************************************************************/
void Sprite::draw(float x, float y, float r, float dt)
{
   render(x, y, r, 0.0, 1.0, dt);
}

/******************************************************************************
 * AnimatedSprite:
 *    INPUT: pGraphics: reference to the graphics object
 *           texture  : filename of the texture to use
 *****************************************************************************/
AnimatedSprite::AnimatedSprite(
     Graphics* pGraphics, string texture, float frameRate)
   : Sprite(pGraphics, texture), mTimeElapsed(0), mFrameRate(frameRate)
{
   mFrameCount = mpTexture->getWidth() / mpTexture->getHeight() ;
}

/******************************************************************************
 * AnimatedSprite:
 *    INPUT: graphics: reference to the graphics object
 *           texture : filename of the texture to use
 *           width   : width  of the sprite (in OpenGL coordinates)
 *           height  : height of the sprite (in OpenGL coordinates)
 *****************************************************************************/
AnimatedSprite::AnimatedSprite(
     Graphics* pGraphics, string texture, int width, int height, 
     float frameRate)
   : Sprite(pGraphics, texture, width, height), mTimeElapsed(0), 
     mFrameRate(frameRate)
{
   mFrameCount = mpTexture->getWidth() / mpTexture->getHeight() ;
}

/******************************************************************************
 * draw: draws the sprite to the screen with the given position and rotation
 *    INPUT: x  : center point x position
 *           y  : center point y position
 *           rot: the sprite's rotation
 *           dt : time elapsed since last draw (in milliseconds)
 *****************************************************************************/
void AnimatedSprite::draw(float x, float y, float r, float dt)
{
   mTimeElapsed += ceil(dt);
   float temp = mTimeElapsed / mFrameRate;
   int frame = (int)(mTimeElapsed / mFrameRate) % mFrameCount;
   float step = 1.0 / (float)mFrameCount;
   render(x, y, r, step * frame, step * (frame + 1), dt);
}

/******************************************************************************
 * TilingSprite:
 *    INPUT: graphics: reference to the graphics object
 *           texture : filename of the texture to use
 *           width   : width  of the sprite (in OpenGL coordinates)
 *           height  : height of the sprite (in OpenGL coordinates)
 *****************************************************************************/
TilingSprite::TilingSprite(
     Graphics* pGraphics, string texture, float width, float height)
   : Sprite(pGraphics, texture, width, height)
{
}

/******************************************************************************
 * draw: draws the sprite to the screen with the given position and rotation
 *    INPUT: x  : center point x position
 *           y  : center point y position
 *           rot: the sprite's rotation
 *           dt : time elapsed since last draw (in milliseconds)
 *****************************************************************************/
void TilingSprite::draw(float x, float y, float r, float dt)
{
   float t = (float)mHeight / (float)mpTexture->getHeight();
   render(x, y, r, 0.0, t, dt);
}

