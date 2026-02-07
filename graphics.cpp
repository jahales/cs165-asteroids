/******************************************************************************
 * graphics.cpp: defines the methods for the Graphics class
 *****************************************************************************/
#include "graphics.h"
#include <GL/glu.h>
#include <time.h>
#include <sstream>
using namespace std;

/******************************************************************************
 * We are drawing the text for score and things like that by hand to make it 
 * look "old school." These are how we render each individual character.
 * Note how -1 indicates "done".  These are paired coordinates where the even
 * are the x and the odd are the y and every 2 pairs represents a point
 *    NOTES: borrowed from Br. Helfrich's uiDraw.cpp
 *****************************************************************************/
const char NUMBER_OUTLINES[10][20] =
{
  {0, 0,  7, 0,   7, 0,  7,10,   7,10,  0,10,   0,10,  0, 0,  -1,-1, -1,-1},//0
  {7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1},//1
  {0, 0,  7, 0,   7, 0,  7, 5,   7, 5,  0, 5,   0, 5,  0,10,   0,10,  7,10},//2
  {0, 0,  7, 0,   7, 0,  7,10,   7,10,  0,10,   4, 5,  7, 5,  -1,-1, -1,-1},//3
  {0, 0,  0, 5,   0, 5,  7, 5,   7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1},//4
  {7, 0,  0, 0,   0, 0,  0, 5,   0, 5,  7, 5,   7, 5,  7,10,   7,10,  0,10},//5
  {7, 0,  0, 0,   0, 0,  0,10,   0,10,  7,10,   7,10,  7, 5,   7, 5,  0, 5},//6
  {0, 0,  7, 0,   7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1},//7
  {0, 0,  7, 0,   0, 5,  7, 5,   0,10,  7,10,   0, 0,  0,10,   7, 0,  7,10},//8
  {0, 0,  7, 0,   7, 0,  7,10,   0, 0,  0, 5,   0, 5,  7, 5,  -1,-1, -1,-1} //9
};

/******************************************************************************
 * drawDigit: Draw a single digit in the old school line drawing style.  
 * The size of the glyph is 8x11 or x+(0..7), y+(0..10)
 *    INPUT: x    : x-coor of the upper left-hand corner
 *           y    : y-coor of the upper left-hand corner
 *           digit: single-character number
 *    NOTES: modifier from Br. Helfrich's uiDraw.h/cpp
 *****************************************************************************/
void drawDigit(float x, float y, char digit)
{
   // Lines won't display when textures are enabled
   glDisable(GL_TEXTURE_2D);

   // we better be only drawing digits
   assert(isdigit(digit));
   if (!isdigit(digit))
      return;

   // compute the row as specified by the digit
   int r = digit - '0';
   assert(r >= 0 && r <= 9);

   // go through each segment.
   for (int c = 0; c < 20 && NUMBER_OUTLINES[r][c] != -1; c += 4)
   {
      assert(NUMBER_OUTLINES[r][c    ] != -1 &&
             NUMBER_OUTLINES[r][c + 1] != -1 &&
             NUMBER_OUTLINES[r][c + 2] != -1 &&
             NUMBER_OUTLINES[r][c + 3] != -1);

      //Draw a line based off of the num structure for each number
      glBegin(GL_LINES);

      glVertex2f(x + NUMBER_OUTLINES[r][c],     y - NUMBER_OUTLINES[r][c + 1]);
      glVertex2f(x + NUMBER_OUTLINES[r][c + 2], y - NUMBER_OUTLINES[r][c + 3]);

      glEnd();
   }

   glEnable(GL_TEXTURE_2D);
}

/******************************************************************************
 * Graphics:
 *    INPUT: width : width of the window
 *           height: height of the window
 *           title : title to display for the window
 *****************************************************************************/
Graphics::Graphics(int width, int height, string title) 
   : mWidth(width), mHeight(height), mIsRunning(true), mTitle(title)
{
   initVideo();
   initOpenGL();
   srand(time(NULL) / 2);

   mLastRenderTime = SDL_GetTicks();
}

/******************************************************************************
 * ~Graphics:
 *****************************************************************************/
Graphics::~Graphics()
{
   // Delete textures
   for (map<string, Texture*>::iterator iter = mTextures.begin();
      iter != mTextures.end(); iter++)
   {
      delete iter->second;
   }
   SDL_GL_DeleteContext(mGLContext);
   SDL_DestroyWindow(mpWindow);
   SDL_Quit(); // Close the SDL window
}

/******************************************************************************
 * drawNumber: Display an positive integer on the screen using the 
 * 7-segment method
 *    INPUT: x     : x-coor of the upper left/right-hand corner
 *           y     : y-coor of the upper left/right-hand corner
 *           number: the number to draw
 *    NOTES: modified from Br. Helfrich's uiDraw.h/cpp
 *****************************************************************************/
void Graphics::drawNumber(float x, float y, unsigned int number, bool rtl)
{
   // render the number as text
   ostringstream sout;
   sout << number;
   string text = sout.str();

   // walk through the text one digit at a time
   if (rtl)
   {
      for (int i = 0; i < text.length(); i++)
      {
         assert(isdigit(text[i]));
         drawDigit(x, y, text[i]);
         x += 11;
      }
   }
   else
   {
      for (int i = text.length() - 1; i >= 0; i--)
      {
         assert(isdigit(text[i]));
         drawDigit(x, y, text[i]);
         x -= 11;
      }
   }
}

/******************************************************************************
 * random: generates a random number between the given values (inclusive)
 *    INPUT: min: minimum value
 *           max: maximum value
 *    NOTES: modified from Br. Helfrich's uiDraw.cpp
 *****************************************************************************/
int Graphics::random(int min, int max)
{
   assert(min <= max);
   int num = (rand() % ((max + 1) - min)) + min;
   assert(min <= num && num <= max);
   return num;
}

/******************************************************************************
 * initOpenGL: initializes the OpenGL settings for 2D rendering
 *****************************************************************************/
void Graphics::initOpenGL()
{  
   // Set the clear 
   glClearColor(0.0, 0.0, 1.0, 0.0); 
   gluOrtho2D(0.0, mWidth, 0.0, mHeight);

   // Initialize settings for transparent 2D texturing
   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glDepthMask(GL_FALSE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/******************************************************************************
 * initVideo: initializes the SDL video for OpenGL rendering
 *    NOTES: developed with help (not copied) from the SDL documentation and
 *    related tutorial sites
 *****************************************************************************/
void Graphics::initVideo()
{
   // Start SDL video subsystem
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
      throw string(SDL_GetError());

   // Set minimum bits per color and for the depth buffer
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

   // Create SDL window with OpenGL context
   mpWindow = SDL_CreateWindow(
      mTitle.c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      mWidth, mHeight,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

   if (mpWindow == NULL)
      throw string(SDL_GetError());

   mGLContext = SDL_GL_CreateContext(mpWindow);

   if (mGLContext == NULL)
      throw string(SDL_GetError());
}

/******************************************************************************
 * run: starts the graphics/event loop 
 *    INPUT: igraphics: interface used to notify the parent of events
 *****************************************************************************/
void Graphics::run(IGraphicsCallback *igraphics)
{
   mpIGraphicsCallback = igraphics;

   while (mIsRunning)
   {
      SDL_Event sdlEvent;

      // Render the scene
      renderScene();

      // Process events
      while (SDL_PollEvent(&sdlEvent))
      {
         switch (sdlEvent.type)
         {
         case SDL_QUIT:
            mIsRunning = false;
            break;
         case SDL_KEYDOWN:
            mpIGraphicsCallback->keyDown(sdlEvent.key.keysym.sym);
            break;
         case SDL_KEYUP:
            mpIGraphicsCallback->keyUp(sdlEvent.key.keysym.sym);
            break;
         }
      }
   }
}

/******************************************************************************
 * renderScene: prepares the scene for rendering and calls the appropriate
 *    IGraphicsCallback interface to render the scene
 *****************************************************************************/
void Graphics::renderScene()
{
   // Calculate the change in time (seconds), making sure it's significant
   long time = 0;
   float dt  = 0;

   while (dt <= 0)
   {
      time = SDL_GetTicks();
      dt = (float)(time - mLastRenderTime) / 1000.0;
   }

   // Clear the display buffers and render the scene
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   mpIGraphicsCallback->renderScene(dt);
   
   SDL_GL_SwapWindow(mpWindow);

   mLastRenderTime = time;
}

/******************************************************************************
 * loadTexture: attempts to load the given texture, first from from 
 *    memory, and if it hasn't been loaded previously, from the filesystem
 *    INPUT : filename: name of the texture to load
 *    OUTPUT: <return>: returns a pointer to the texture
 *****************************************************************************/
Texture* Graphics::loadTexture(std::string filename)
{
   map<string, Texture*>::iterator iter = mTextures.find(filename);

   // Try to load the texture by name
   if (iter != mTextures.end())
   {
      return iter->second;
   }
   // Otherwise attempt to load it from the filesystem
   else
   {
      try
      {
         Texture* pTexture = new Texture(filename);

         mTextures.insert(mTextures.begin(),
            std::pair<string, Texture*>(filename, pTexture));

         return pTexture;
      }
      catch (string ex)
      {
         throw ex; // FATAL EXCEPTION
      }
   }
}
