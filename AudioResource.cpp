#include "AudioResource.h"
#include "GameManager.h"
using namespace std;

AudioResource::AudioResource(uint32 id, std::string scope, std::string file_name, GameResourceType type, AudioType audio_type, GameManager* gm) : GameResource(id, scope, file_name, type)
{
   game_manager = gm;
   this->audio_type = audio_type;
   ar_info = game_manager->createAudioResourceInfo();
}

AudioResource::~AudioResource()
{
   free(ar_info);
   game_manager = NULL;
}

AudioResourceInfo* AudioResource::getAudioResourceInfo()
{
   return ar_info;
}

void AudioResource::load()
{
   if (audio_type == SAMPLE)
   {
      game_manager->loadSampleAudioResource(getResourceFileName(), ar_info);
   }
   else 
   {
      game_manager->loadStreamAudioResource(getResourceFileName(), ar_info);
   }
}

void AudioResource::unload()
{
   if (audio_type == SAMPLE)
   {
      game_manager->unloadSampleAudioResource(ar_info);
   }
   else 
   {
      game_manager->unloadStreamAudioResource(ar_info);
   }
}
