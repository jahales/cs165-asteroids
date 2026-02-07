/******************************************************************************
 * texture.h: defines the Texture class
 *****************************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <assert.h>

/******************************************************************************
 * Texture: stores OpenGL textures and basic state info.
 *****************************************************************************/
class Texture
{
private:

   GLuint      mId;
   int         mHeight;
   int         mWidth;
   std::string mFilename;

public:

   /***************************************************************************
    * Texture:
    *    INPUT: filename: name of the texture file (bitmap)
    *    NOTES: developed with help (not copied) from the SDL documentation and
    *    related tutorial sites
    **************************************************************************/
   Texture(std::string filename);

   /***************************************************************************
    * ~Texture()
    **************************************************************************/
   ~Texture();

   /***************************************************************************
    * Getters
    **************************************************************************/
   int getWidth()  const { return mWidth;  }
   int getHeight() const { return mHeight; }
   GLuint getId()  const { return  mId;    }
};

#endif