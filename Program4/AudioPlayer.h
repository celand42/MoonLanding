#if !defined (AUDIO_PLAYER)
#define AUDIO_PLAYER

#include "AudioResource.h"

class AudioPlayer
{
   private:
      AudioResource* audio_resource;

      int num_repeats;  // 0 = play once, 1 = play twice, etc.
      int repeat_count;

      //store a list of listeners to notify when the audio completes all repetitions

   public:
      AudioPlayer(AudioResource* audio_resource, int num_plays);
      virtual ~AudioPlayer();

      int getNumRepeats();
      int getRepeatCount();
      void incRepeatCount();
      AudioResource* getAudioResource();

      //add listener method

      virtual void onMediaComplete();
};

#endif
