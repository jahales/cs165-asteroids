/***************************************************************************
 * audio.h: defines the AudioManager class for loading/playing audio files
 **************************************************************************/
#ifndef AUDIO_H
#define AUDIO_H

#include "sound.h"
#include <map>
#include <string>
#include <list>
#include <SDL2/SDL.h>

/***************************************************************************
 * AudioManager: manages loading and playing audio files. Files are loaded
 *    into memory on-demand and indexed by filename. Sounds can be played 
 *    a single time or set as the loop background sound
 **************************************************************************/
class AudioManager
{
private:

   bool                     mLoaded;
   int                      mVolume;
   SDL_AudioSpec            mAudioSpec;
   std::map<std::string, Sound*> mSoundMap;
   std::list<PlaybackInfo*> mSoundList;   

   /***************************************************************************
    * load: retrieves the PlaybackInfo for the given file. If the file has not
    *    yet been loaded, the method will attempt to load the file.
    *    INPUT : filename: name of the file to load
    *    OUTPUT: <return>: The PlaybackInfo associated with the file, or NULL
    **************************************************************************/
   PlaybackInfo* load(std::string filename, bool loop = false);

   /***************************************************************************
    * mixAudio: fills the audio stream with a mix of all the current sounds
    *    INPUT: audio : audio stream to be sent to the sound card
    *           length: number of bytes of audio the stream is requesting
    **************************************************************************/
   void mixAudio(Uint8 *audio, int length);

   /***************************************************************************
    * audioCallback: called when the sound card request the next batch of sound
    *    data. This will invoke the mixAudio method
    *    INPUT: userData: void pointer containing the class reference
    *           audio   : audio stream to be sent to the sound card
    *           length  : number of bytes of audio the stream is requesting
    **************************************************************************/
   static void audioCallback(void *userData, Uint8 *audio, int length);

public:

   AudioManager();

   ~AudioManager();

   /***************************************************************************
    * setVolume: set the current volume
    *    INPUT: volume: new value for the volume
    **************************************************************************/
   void setVolume(int volume);

   /***************************************************************************
    * adjustVolume: increment the volume
    *    INPUT: increment: amount to change the volume
    **************************************************************************/
   void adjustVolume(int increment);

   /***************************************************************************
    * pause: sets the pause status of all sounds
    *    INPUT: paused: whether or not to pause all of the sounds
    **************************************************************************/
   void pause(bool paused = true);

   /***************************************************************************
    * play: plays the given file a single time through
    *    INPUT: filename: name of the file to play
    **************************************************************************/
   void play(std::string filename, bool loop = false);

   /***************************************************************************
    * stop: stops all the audio
    **************************************************************************/
   void stop();

   /***************************************************************************
    * stop: stops the given audio file from playing (thread-safe)
    *    INPUT: filename: name of the file to stop
    **************************************************************************/
   void stop(std::string filename);
};

#endif