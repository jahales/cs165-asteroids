/***************************************************************************
 * audio.cpp: method definitions for the AudioManager class
 **************************************************************************/
#include "audio.h"
using namespace std;

/******************************************************************************
 * Constructor: initialize SDL and SDL_Sound, get device settings
 *****************************************************************************/
AudioManager::AudioManager()
{
   // Set defaults
   mLoaded = false;
   mVolume = SDL_MIX_MAXVOLUME / 2;

   // Audio format specifications
   SDL_AudioSpec desired;

   // Initialize SDL audio subsystem
   if (SDL_Init(SDL_INIT_AUDIO) != 0)
   {
      throw string("Unable to initialize SDL Audio");
   }

   // Open the audio device, attempting to get the desired format
   desired.freq     = 44100;
   desired.format   = AUDIO_S16;
   desired.samples  = 4096;
   desired.channels = 2;
   desired.callback = &audioCallback;
   desired.userdata = this;

   // Is the error somewhere here?
   if (SDL_OpenAudio(&desired, &mAudioSpec) < 0)
   {
      throw string(SDL_GetError()) ;
   }

   pause(false); // Started in paused mode, unpause

   mLoaded = true; // If sound fails to initialize, continue quietly
}

/******************************************************************************
 * Destructor: delete audio data
 *****************************************************************************/
AudioManager::~AudioManager()
{
   // Stop all audio from playing (blocks until stopped)
   stop();

   // Pause and lock audio to free resources
   SDL_PauseAudio(true);    
   SDL_LockAudio();

   // Clean-up 
   for (list<PlaybackInfo*>::iterator iter = mSoundList.begin();
      iter != mSoundList.end(); iter++)
   {
      delete *iter;
   }
   for (map<string, Sound*>::iterator iter = mSoundMap.begin(); 
      iter != mSoundMap.end(); iter++)
   {
      delete iter->second; // Delete sounds
   }

   // Done, unlock and close SDL
   SDL_UnlockAudio();
     
   SDL_CloseAudio();
}

/******************************************************************************
 * load: retrieves the PlaybackInfo for the given file. If the file has not
 *    yet been loaded, the method will attempt to load the file.
 *    INPUT : filename: name of the file to load
 *    OUTPUT: <return>: The PlaybackInfo associated with the file, or NULL
 *****************************************************************************/
PlaybackInfo* AudioManager::load(string filename, bool loop)
{
   map<string, Sound*>::iterator iter = mSoundMap.find(filename);
   Sound* pSound = NULL;

   // Try to load the file by name
   if (iter != mSoundMap.end())
   {
      pSound = iter->second;
   }
   // Otherwise attempt to load it 
   else
   {
      try
      {
         pSound = new Sound(filename, &mAudioSpec);
         mSoundMap.insert(mSoundMap.begin(), 
            std::pair<string, Sound*>(filename, pSound));
      }
      catch (string ex) 
      {
         if (mLoaded)
            throw ex;
         else return NULL; // Fail silently if audio failed to load
      }
   }

   return pSound->getPlaybackInfo(loop);
}

/******************************************************************************
 * setVolume: set the current volume
 *    INPUT: volume: new value for the volume
 *****************************************************************************/
void AudioManager::setVolume(int volume)
{
   if (volume < 0 || volume >= SDL_MIX_MAXVOLUME)
   {
      throw string("Volume out of range");
   }
   mVolume = volume;
}

/******************************************************************************
 * adjustVolume: increment the volume
 *    INPUT: increment: amount to change the volume
 *****************************************************************************/
void AudioManager::adjustVolume(int increment)
{
   mVolume += increment;

   if (mVolume < 0)
   {
      mVolume = 0;
   }
   else if (mVolume >= SDL_MIX_MAXVOLUME)
   {
      mVolume = SDL_MIX_MAXVOLUME - 1;
   }
}

/******************************************************************************
 * pause: sets the pause status of all sounds
 *    INPUT: paused: whether or not to pause all of the sounds
 *****************************************************************************/
void AudioManager::pause(bool paused)
{
   SDL_PauseAudio(paused);
}

/***************************************************************************
 * play: plays the given file a single time through
 *    INPUT: filename: name of the file to play
 **************************************************************************/
void AudioManager::play(string filename, bool loop)
{
   PlaybackInfo* pSound = load(filename, loop);

   if (pSound != NULL)
   {
      mSoundList.push_back(pSound);
   }
}

/******************************************************************************
 * stop: stops all the audio
 *****************************************************************************/
void AudioManager::stop()
{
   for (list<PlaybackInfo*>::iterator iter = mSoundList.begin();
      iter != mSoundList.end();
      iter++)
   {
      (*iter)->stop();
   }
}

/***************************************************************************
 * stop: stops the given audio file from playing (thread-safe)
 *    INPUT: filename: name of the file to stop
 **************************************************************************/
void AudioManager::stop(std::string filename)
{
   PlaybackInfo* temp = load(filename, false);

   // Search for PlaybackInfo's that share the same Sound instance
   for (list<PlaybackInfo*>::iterator iter = mSoundList.begin();
      iter != mSoundList.end(); iter++)
   {
      if (*temp == *iter)
         (*iter)->stop();
   }

   delete temp;
}

/******************************************************************************
 * audioCallback: called when the sound card request the next batch of sound
 *    data. This will invoke the mixAudio method
 *    INPUT: userData: void pointer containing the class reference
 *           audio   : audio stream to be sent to the sound card
 *           length  : number of bytes of audio the stream is requesting
 *****************************************************************************/
void AudioManager::audioCallback(void *userData, Uint8 *audio, int length)
{
   ((AudioManager*)userData)->mixAudio(audio, length);
}

/******************************************************************************
 * mixAudio: fills the audio stream with a mix of all the current sounds
 *    INPUT: audio : audio stream to be sent to the sound card
 *           length: number of bytes of audio the stream is requesting
 *****************************************************************************/
void AudioManager::mixAudio(Uint8 *audio, int length)
{
   if (!mLoaded)
      return; // Sound must have failed to initialize

   // Prepare audio buffer
   memset(audio, 0, length);
   
   // Play all the sounds in the list
   for (list<PlaybackInfo*>::iterator iter = mSoundList.begin(); 
      iter != mSoundList.end(); )
   {
      PlaybackInfo* pSound = *iter;
      
      pSound->play(audio, length, mVolume);

      if (!pSound->isPlaying())
      {
         delete *iter;
         iter = mSoundList.erase(iter);
      }
      else
      {
         iter++; // ... Avoid incrementing after deletes
      }
   }
}