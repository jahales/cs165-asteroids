/******************************************************************************
 * graphics.h: defines the IGraphicsCallback and Graphics class which are used
 *    to render the graphics and interface with the SDL library
 *****************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <string>
#include <map>
#include <ctime>
#include "texture.h"

/******************************************************************************
 * IGraphicsCallback: callback interface used to notifiy client of graphcis
 *    events such as drawing and key presses.
 *****************************************************************************/
class IGraphicsCallback
{
public:

   /***************************************************************************
    * renderScene: triggers the renderScene event callback
    *    INPUT: dt: number of milliseconds elapsed since last render
    **************************************************************************/
   virtual void renderScene(float dt) = 0;

   /***************************************************************************
    * keyUp: triggered when a key is released
    *    INPUT: key: the ascii character value
    **************************************************************************/
   virtual void keyUp(SDL_Keycode key)     = 0;

   /***************************************************************************
    * keyDown: triggered when a key is pressed
    *    INPUT: key: the ascii character value
    **************************************************************************/
   virtual void keyDown(SDL_Keycode key)   = 0;
};

/******************************************************************************
 * Graphics: interfaces with the SDL library to render graphics
 *****************************************************************************/
class Graphics
{
private:

   int                mWidth;
   int                mHeight;
   bool               mIsRunning;
   std::string        mTitle;
   clock_t            mLastRenderTime;
   IGraphicsCallback* mpIGraphicsCallback;
   SDL_Window*        mpWindow;
   SDL_GLContext      mGLContext;

   std::map<std::string, Texture*> mTextures;

   /***************************************************************************
    * renderScene: prepares the scene for rendering and calls the appropriate
    *    IGraphicsCallback interface to render the scene
    **************************************************************************/
   void renderScene();

   /***************************************************************************
    * initVideo: initializes the SDL video for OpenGL rendering
    *    NOTES: developed with help (not copied) from the SDL documentation and
    *    related tutorial sites
    **************************************************************************/
   void initVideo();

   /***************************************************************************
    * initOpenGL: initializes the OpenGL settings for 2D rendering
    **************************************************************************/
   void initOpenGL();

public:

   /***************************************************************************
    * Graphics:
    *    INPUT: width : width of the window
    *           height: height of the window
    *           title : title to display for the window
    **************************************************************************/
   Graphics(int width = 200, int height = 200, std::string title = "");

   /***************************************************************************
    * ~Graphics:
    **************************************************************************/
   ~Graphics();

   /***************************************************************************
    * getters:
    **************************************************************************/
   float getWidth()       const { return (float) mWidth; }
   float getHeight()      const { return (float)mHeight; }
   std::string getTitle() const { return         mTitle;  }

   /***************************************************************************
    * run: starts the graphics/event loop 
    *    INPUT: igraphics: interface used to notify the parent of events
    **************************************************************************/
   void run(IGraphicsCallback *igraphics);

   /***************************************************************************
    * loadTexture: attempts to load the given texture, first from from 
    *    memory, and if it hasn't been loaded previously, from the filesystem
    *    INPUT : filename: name of the texture to load
    *    OUTPUT: <return>: returns a pointer to the texture
    **************************************************************************/
   Texture* loadTexture(std::string filename);

   /***************************************************************************
    * random: generates a random number between the given values (inclusive)
    *    INPUT: min: minimum value
    *           max: maximum value
    *    NOTES: modified from Br. Helfrich's uiDraw.cpp
    **************************************************************************/
   static int random(int min, int max);

   /***************************************************************************
    * drawNumber: Display an positive integer on the screen using the 
    * 7-segment method
    *    INPUT: x     : x-coor of the upper left/right-hand corner
    *           y     : y-coor of the upper left/right-hand corner
    *           number: the number to draw
    *    NOTES: modified from Br. Helfrich's uiDraw.h/cpp
    **************************************************************************/
   void drawNumber(float x, float y, unsigned int number, bool rtl = true);
};

#endif