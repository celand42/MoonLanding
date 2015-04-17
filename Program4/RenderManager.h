#if !defined RENDER_MANAGER
#define RENDER_MANAGER

#include "GameHeader.h"
#include "Ogre.h"
#include "RenderListener.h"
#include "CSC2110/ListArray.h"
#include "tinyxml.h"

class GameManager;
class GUIManager;
class ScriptManager;
class PhysicsManager;
class Saber;

struct SceneNodeMotion
{
   Ogre::SceneNode* scene_node;
};
struct DebugDrawer
{
      Ogre::SceneNode* manual_object_node;
      Ogre::ManualObject* manual_object;
};

class RenderManager
{
   private:

      Ogre::SceneNode* selected_node;

      ListArray<Ogre::AnimationState>* animation_states;

      Ogre::Root* root;
      Ogre::RenderWindow* window;
      Ogre::SceneManager* scene_manager;

      Ogre::Camera* camera;
      Ogre::Viewport* viewport;

      PhysicsManager* physics_manager;
      ScriptManager* script_manager;
      GUIManager* gui_manager;
      GameManager* game_manager;
	  Saber* saber;

      RenderListener* animation_render_listener;
      RenderListener* input_render_listener;
      RenderListener* physics_render_listener;

      void init();
      size_t window_handle;
      Ogre::Real time_since_last_frame;

      std::string loaded_group;

      void addSceneNodeChildren(TiXmlNode* xml_node, Ogre::SceneNode* parent_node, float* values);
      void addSceneNodeAnimation(TiXmlNode* animation_node_xml, Ogre::SceneNode* child_scene_node, std::string animation_name_text, float* values);

   public:
      RenderManager(GameManager* game_manager);
      virtual ~RenderManager();

      void updateAudio();
      void mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);
      void mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);

      void mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y);
      void joystickAxisMoved(int* amount);

	  void createScene(std::string fileName);
	  void processScene(TiXmlElement* elements, Ogre::SceneNode* parent_node);
	    	  
      size_t getRenderWindowHandle();
      int getRenderWindowWidth();
      int getRenderWindowHeight();
      void checkForInput(float time_step);

      void addPathResource(std::string file_name, std::string path_type, std::string group_name);
      void addMeshResource(std::string file_name, std::string mesh_str, std::string group_name);
      void loadResourceGroup(std::string section_name);
      void unloadResourceGroup(std::string group_name);
      void initialiseResourceGroup(std::string group_name);

      //void loadOgreResourcesFromXML(std::string file_name, std::string section_name);
      //void unloadOgreResources();

      void buildSceneFromXML(std::string file_name);

      void setTimeSinceLastFrame(Ogre::Real time_since_last_frame);

      Ogre::RenderWindow* getRenderWindow();
      Ogre::SceneManager* getSceneManager();

      void startRendering();
      void stopRendering();

      void processAnimations(float time_step);
	  void resetAnimation();
	  void keyPressed(std::string game_key);
	  void talk();
	  
      void setSelectedNode(std::string item);
      void logComment(std::string comment_message);
      void executeRotateScript(std::string file_name, std::string script_name, std::string object_name, int degrees);

      void setOrientation(SceneNodeMotion* scene_node_motion, double w, double x, double y, double z);
      void setPosition(SceneNodeMotion* scene_node_motion, double x, double y, double z);
      float* getOrientation(SceneNodeMotion* scene_node_motion);
      float* getPosition(SceneNodeMotion* scene_node_motion);

      void applyTorque(std::string name, float pitch, float yaw, float roll);
      void stepPhysicsSimulation(float elapsed_time);
      DebugDrawer* createOgreBulletDebugDrawer();
      void drawLine(DebugDrawer* db, float* from_values, float* to_values, float* color_values, int* count);
      void clearLines(DebugDrawer* db);
      void destroySceneNodeMotion(SceneNodeMotion* snm);
      void destroyDebugDrawer(DebugDrawer* debug_d);
};

#endif
