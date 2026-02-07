#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include <assert.h>
using std::string;

/***************************************************************************
 * Forward declarations
 **************************************************************************/
class Sound;
class PlaybackInfo;

/***************************************************************************
 * PlaybackInfo: contains necessary data for playing an audio file. This 
 *    prevents needing complete copies of the Sound class
 **************************************************************************/
class PlaybackInfo
{
private:

   friend class Sound; // Allow Sound to create instances of this class

   int          mPosition;
   bool         mIsPlaying;
   bool         mLoop;
   const Sound *mpSound;

   /***************************************************************************
    * PlaybackInfo: special constructor, only Sound will be making instances
    *    of this class
    **************************************************************************/
   PlaybackInfo(const Sound * pSound, bool loop) 
      : mPosition(0), mpSound(pSound), mIsPlaying(false), mLoop(loop) {  }

public:

   /***************************************************************************
    * isPlaying: determines whether or not the current sound is playing
    *    OUTPUT: <return>: returns true if the sound is currently being played
    **************************************************************************/
   bool isPlaying() const { return mIsPlaying; }

   /***************************************************************************
    * play: mixes the sound data into the audio stream 
    *    INPUT : audio:  audio stream to mix into
    *            length: maximum length of the sound data to mix
    *            volume: volume of the sound
    **************************************************************************/
   void play(Uint8 *audio, int length, int volume);

   /***************************************************************************
    * stop: stops the sound and disables looping (thread-safe)
    **************************************************************************/
   void stop();

   /***************************************************************************
    * operator==: checks if the Sound instances of two PlaybackInfo's are the
    *    same. The right-hand side is generally a pointer returned by an
    *    iterator, so this is an overload for convenience
    *    INPUT : rhs     : pointer to a PlaybackInfo to compare to
    *    OUTPUT: <return>: true if both point to the same sound
    **************************************************************************/
   bool operator==(const PlaybackInfo *rhs) { return mpSound == rhs->mpSound; }
};

/***************************************************************************
 * Sound: contains sample data for WAV sound files
 **************************************************************************/
class Sound
{
public:

   friend class PlaybackInfo; // PlaybackInfo is a special abstraction

   string mFilename;
   Uint32 mLength;
   Uint8* mpSamples;          // Raw PCM sample data

   /***************************************************************************
    * load: loads the specified audio file and converts it to the specified
    *    sample format
    *    INPUT : filename: name of the WAVE file to load
    *            spec    : Structure representing the audio format
    *    NOTES: developed with help (not copied) from the SDL documentation and
    *    related tutorial sites
    **************************************************************************/
   void load(string filename, SDL_AudioSpec *spec);

   /***************************************************************************
    * mix: mixes the sound data into the audio stream until the end of the 
    *    sound data
    *    INPUT : audio:  audio stream to mix into
    *            length: maximum length of the sound data to mix
    *            volume: volume of the sound
    **************************************************************************/
   void mix(Uint8 *audio, int length, int volume, int &position) const;

   /***************************************************************************
    * loop: mixes the sound data into the audio stream, filling the buffer with
    *    looping sound data
    *    INPUT : audio:  audio stream to mix into
    *            length: absolute length of the sound data to mix
    *            volume: volume of the sound
    **************************************************************************/
   void loop(Uint8 *audio, int length, int volume, int &position) const;

public:

   /***************************************************************************
    * Sound: initializes an instance of Sound from a WAV file
    *    INPUT: filename: name of the file to load
    *           spec    : format settings for the WAV file
    **************************************************************************/ 
   Sound(string filename, SDL_AudioSpec *spec);

   /***************************************************************************
   * ~Sound: clean-up
   **************************************************************************/
   ~Sound();

   /***************************************************************************
    * Getters
    **************************************************************************/ 
   string getFilename() const { return mFilename;  }

   /***************************************************************************
    * getPlaybackInfo: retrieves a structure containing playback info for the 
    *    Sound file. It mostly keeps track of current position, etc. without 
    *    copying the raw sound data.
    *    INPUT : loop    : whether to loop the sound or play it once through
    *    OUTPUT: <return>: pointer to the new PlaybackInfo instance
    **************************************************************************/
   PlaybackInfo* getPlaybackInfo(bool loop) const;
};

#endif