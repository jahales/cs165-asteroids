/******************************************************************************
 * sound.cpp: implements methods for PlaybackInfo and Sound. Audio/Sound 
 *    functions developed with some assistance from online code:
 *    "Code of Programming Linux Games" (GNU-Licensed)
 *    http://sdl-sparks.googlecode.com/hg/CodeOfProgrammingLinuxGames/sdl
 *    Sound::load was mostly ported from C, other functions mostly developed
 *    independently
 *****************************************************************************/
#include "sound.h"
using namespace std;

/******************************************************************************
 * PlaybackInfo: method implementations
 *****************************************************************************/

/******************************************************************************
 * stop: stops the sound and disables looping (thread-safe)
 *****************************************************************************/
void PlaybackInfo::stop() 
{
   mLoop = false;
   mPosition = mpSound->mLength; 
}

/******************************************************************************
 * play: mixes the sound data into the audio stream 
 *    INPUT : audio:  audio stream to mix into
 *            length: maximum length of the sound data to mix
 *            volume: volume of the sound
 *****************************************************************************/
void PlaybackInfo::play(Uint8 *audio, int length, int volume)
{
   mIsPlaying = true;

   if (mLoop)
   {
      mpSound->loop(audio, length, volume, mPosition);   
   }
   else
   {
      mpSound->mix(audio, length, volume, mPosition);
      mIsPlaying = (mPosition < mpSound->mLength);
   }
}

/******************************************************************************
 * Sound: method implementations
 *****************************************************************************/

/******************************************************************************
 * Sound: initializes an instance of Sound from a WAV file
 *    INPUT: filename: name of the file to load
 *           spec    : format settings for the WAV file
 *****************************************************************************/ 
Sound::Sound(string filename, SDL_AudioSpec *spec)
{
   mFilename = filename;
   mLength = 0;
   mpSamples = NULL;
   load(filename, spec);
}

/******************************************************************************
 * ~Sound: clean-up
 *****************************************************************************/
Sound::~Sound()
{
   delete [] mpSamples;
}

/******************************************************************************
 * getPlaybackInfo: retrieves a structure containing playback info for the 
 *    Sound file. It mostly keeps track of current position, etc. without 
 *    copying the raw sound data.
 *    INPUT : loop    : whether to loop the sound or play it once through
 *    OUTPUT: <return>: pointer to the new PlaybackInfo instance
 *****************************************************************************/
PlaybackInfo* Sound::getPlaybackInfo(bool loop) const
{ 
   return new PlaybackInfo(this, loop); 
}

/******************************************************************************
 * mix: mixes the sound data into the audio stream until the end of the 
 *    sound data
 *    INPUT : audio:  audio stream to mix into
 *            length: maximum length of the sound data to mix
 *            volume: volume of the sound
 *****************************************************************************/
void Sound::mix(Uint8 *audio, int length, int volume, int &position) const
{
   assert(volume  > 0 && volume < SDL_MIX_MAXVOLUME);
   assert(length >= 0 && audio != NULL);

   // Select current sample and don't play past end
   Uint8* pSample = mpSamples + position;      
   
   if (position + length >= mLength)  
   {
      length = mLength - position; 
      assert(length >= 0);
   }

   // Mix audio and update audio status
   SDL_MixAudio(audio, pSample, length, volume); 
   position += length;   
}

/******************************************************************************
 * loop: mixes the sound data into the audio stream, filling the buffer with
 *    looping sound data
 *    INPUT : audio:  audio stream to mix into
 *            length: absolute length of the sound data to mix
 *            volume: volume of the sound
 *****************************************************************************/
void Sound::loop(Uint8 *audio, int length, int volume, int &position) const
{
   assert(volume  > 0 && volume < SDL_MIX_MAXVOLUME);
   assert(length >= 0 && audio != NULL);

   // Fill the audio buffer with looping audio data 
   for (int nMixed = 0; nMixed < length; )
   {
      Uint8* pSample = mpSamples + position;
      int writeLength = length - nMixed;

      // If the sound would end, wrap back to the beginning
      if (position + writeLength >= mLength)
      {
         writeLength = mLength - position;
         position = 0; 
         assert(writeLength >= 0);
      }
      // Otherwise advance as normal
      else
      {
         position += writeLength;
         assert(position <= mLength);
      }

      // Mix the current section of audio data
      SDL_MixAudio(audio + nMixed, pSample, writeLength, volume);
      nMixed += writeLength;
      assert(nMixed <= length);
   }
}

/***************************************************************************
 * load: loads the specified audio file and converts it to the specified
 *    sample format
 *    INPUT : filename: name of the WAVE file to load
 *            spec    : Structure representing the audio format
 *    NOTES: developed with help (not copied) from the SDL documentation and
 *    related tutorial sites
**************************************************************************/
void Sound::load(string filename, SDL_AudioSpec *spec)
{
   assert(filename.length() > 0);
   assert(spec != NULL);

   SDL_AudioCVT cvt;     // Audio format conversion structure
   SDL_AudioSpec loaded; // Format of the loaded data
   Uint8* pBuffer;       // Buffer used for conversion

   // Load the file in its original format
   if (SDL_LoadWAV(filename.c_str(), &loaded, &mpSamples, &mLength) == NULL)
   {
      throw string("Unable to load sound file: ") + filename;
   }

   // Build a conversion structure for converting the samples
   if (SDL_BuildAudioCVT(&cvt, 
      loaded.format, loaded.channels, loaded.freq,
       spec->format,  spec->channels,  spec->freq) < 0)
   {
      throw string("Unable to convert sound ") + filename;
   }
   
   // Allocate a new buffer (since sample size varies)
   cvt.len = mLength;

   try
   {
      pBuffer = new Uint8[cvt.len * cvt.len_mult / sizeof(Uint8)];
   }
   catch (bad_alloc ex)
   {
      SDL_FreeWAV(mpSamples); // Release resources
      throw string("Memory allocation error loading file: ") + filename;
   }

   // Copy the samples and perform the conversion
   memcpy(pBuffer, mpSamples, mLength * sizeof(Uint8));
   cvt.buf = pBuffer;

   if (SDL_ConvertAudio(&cvt) < 0)
   {
      delete [] pBuffer;
      SDL_FreeWAV(mpSamples);
      throw string(SDL_GetError());
   }

   // Swap old and new buffers and set new size
   SDL_FreeWAV(mpSamples);
   mpSamples = pBuffer;
   mLength *= cvt.len_mult;
}