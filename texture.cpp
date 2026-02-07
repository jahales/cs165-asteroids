/******************************************************************************
 * Texture.cpp: defines the Texture methods
 *****************************************************************************/
#include "texture.h"
using namespace std;

/******************************************************************************
 * ~Texture()
 *****************************************************************************/
Texture::~Texture()
{
   glDeleteTextures(1, &mId);
}

/******************************************************************************
 * Texture: (developed with help from the SDL documentation
 *    INPUT: filename: name of the texture file (bitmap)
 *    NOTES: developed with help (not copied) from the SDL documentation and
 *       related tutorial sites
 *****************************************************************************/
Texture::Texture(string filename) : mFilename(filename)
{
   // Attemp to load the bitmap file as an SDL surface
   SDL_Surface *surface = SDL_LoadBMP(filename.c_str());

   if (surface == NULL)
      throw string("Error reading file " + filename + ": ") + SDL_GetError();

   // Determine the color format (Alpha/No Alpha? RGB/GBR?)
   GLuint nColors = surface->format->BytesPerPixel;
   GLenum format = 0;

   if (nColors == 4)    
   {
      format = (surface->format->Rmask == 0xFF) ? GL_RGBA : GL_BGRA;
   }
   else if (nColors == 3) 
   {
      format = (surface->format->Rmask == 0xFF) ? GL_RGB  : GL_BGR;
   }
   else                                        
   {
      SDL_FreeSurface(surface);
      throw string("Invalid image color format: " + filename);
   }

   // Generate a texture handle and bind it
   glGenTextures(1, &mId);
   glBindTexture(GL_TEXTURE_2D, mId);

   // Set the texture's stretching properties and image data
   mWidth  = surface->w;
   mHeight = surface->h;

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, nColors, surface->w, surface->h, 0,
      format, GL_UNSIGNED_BYTE, surface->pixels);

   // Free resources and create a new Texture instance
   SDL_FreeSurface(surface);
}
