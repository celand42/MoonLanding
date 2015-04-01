#if !defined GAME_MANAGER
#define GAME_MANAGER

#include "GameHeader.h"
#include "CSC2110/ListArray.h"
#include "Ogre.h"
#include "InputListener.h"
#include "tinyxml.h"
#include <string>

class RenderManager;
class ResourceManager;
class LogManager;
class InputManager;
class AudioManager;
class Saber;
struct AudioResourceInfo;

//supplies communication between managers
class GameManager : public InputListener
{
   private:
      RenderManager* render_manager;
      ResourceManager* resource_manager;
      LogManager* log_manager;
      InputManager* input_manager;
      AudioManager* audio_manager;
	  Saber* saber;

      GameManager(std::string scene_file_name);
      void init(std::string scene_file_name);

   public:
      virtual ~GameManager();
      static GameManager* getGameManager(std::string scene_file_name);

	  void processAnimations(float time_step, ListArray<Ogre::AnimationState>* animation_states);
	  void resetAnimation();
	  
      void updateAudio();
      void playAudio(uint32 audio_id, uint32 num_repeats);

      void loadResources(std::string scope_text);

      int getRenderWindowWidth();
      int getRenderWindowHeight();
      size_t getRenderWindowHandle();
      void checkForInput(float time_step);

      void joystickButtonPressed(std::string button);
      void joystickAxisMoved(int* amount);
      void keyPressed(std::string game_key);
      void keyReleased(std::string game_key);
      void mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);
      void mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);
      void mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y);

      void logProblem(std::string progress_message, int line_number);
      void logComment(std::string comment_message);

      void addPathResource(std::string file_name, std::string path_type, std::string group_name);
      void addMeshResource(std::string file_name, std::string, std::string group_name);
      void loadRenderResourceGroup(std::string group_name);
      void unloadRenderResourceGroup(std::string group_name);
      void initialiseRenderResourceGroup(std::string group_name);

      AudioResourceInfo* createAudioResourceInfo();
      void loadSampleAudioResource(std::string file_name, AudioResourceInfo* ar_info);
      void loadStreamAudioResource(std::string file_name, AudioResourceInfo* ar_info);
      void unloadSampleAudioResource(AudioResourceInfo* ar_info);
      void unloadStreamAudioResource(AudioResourceInfo* ar_info);

      static int parseInt(std::string& str);
      static float parseFloat(std::string& str);
      static void parseFloats(std::string& str, float* values);
      static std::string textFromChildNode(TiXmlNode* parent_node, const char* child_element_name);
};

#endif
