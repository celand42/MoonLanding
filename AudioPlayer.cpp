#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(AudioResource* audio_resource, int num_plays)
{
   this->audio_resource = audio_resource;
   this->num_repeats = num_plays - 1;  //if num_repeats is 0, then the sound plays once
   this->repeat_count = 0;
}

AudioPlayer::~AudioPlayer()
{
   audio_resource = NULL;
}

AudioResource* AudioPlayer::getAudioResource()
{
   return audio_resource;
}

int AudioPlayer::getNumRepeats()
{
   return num_repeats;
}

int AudioPlayer::getRepeatCount()
{
   return repeat_count;
}

void AudioPlayer::incRepeatCount()
{
   repeat_count++;
}

//delete the audio player, but not the audio resource
//this can also act as a callback function to notify listeners that the audio has completed
void AudioPlayer::onMediaComplete()
{
   delete this;
}
