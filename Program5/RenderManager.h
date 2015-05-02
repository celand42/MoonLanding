#if !defined RENDER_MANAGER
#define RENDER_MANAGER

#include "GameHeader.h"
#include "Ogre.h"
#include "RenderListener.h"
#include "CSC2110/ListArray.h"
#include "tinyxml.h"
#include <string>

class GameManager;
class GUIManager;
class ScriptManager;
class PhysicsManager;
class NetworkManager;

struct SceneNodeMotion
{
   Ogre::SceneNode* scene_node_motion;
};

struct SceneNodeManual
{
   Ogre::SceneNode* scene_node_manual;
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

      RenderListener* animation_render_listener;
      RenderListener* input_render_listener;
      RenderListener* physics_render_listener;
      RenderListener* network_render_listener;

      void init();
      size_t window_handle;
      Ogre::Real time_since_last_frame;

      std::string loaded_group;

      void addSceneNodeChildren(TiXmlNode* xml_node, Ogre::SceneNode* parent_node, float* values);
      void addSceneNodeAnimation(TiXmlNode* animation_node_xml, Ogre::SceneNode* child_scene_node, std::string animation_name_text, float* values);

/*
      void buildSubmarineAnimation(Ogre::SceneNode* submarine_animation_node);
      void buildRudderAnimation(Ogre::SceneNode* rudder_animation_node);
      void buildPeriscopeAnimation(Ogre::SceneNode* periscope_animation_node);
      void buildPropellerAnimation(Ogre::SceneNode* propeller_animation_node);

      void buildXWingAnimationScene();
      void buildRobotAnimation(Ogre::SceneNode* robot_node);
      void buildStarShipAnimation(Ogre::SceneNode* starShip);
      void buildURWingAnimation(Ogre::SceneNode* URWing);
      void buildLRWingAnimation(Ogre::SceneNode* LRWing);
      void buildULWingAnimation(Ogre::SceneNode* ULWing);
      void buildLLWingAnimation(Ogre::SceneNode* ULWing);

      void buildCannonSceneGraph();
      void buildBarrelAnimation(Ogre::SceneNode* barrel_animation_node);
      void buildWheelAnimation(Ogre::SceneNode* wheel_animation_node);
      void buildCannonballAnimation(Ogre::SceneNode* cannonball_animation_node);
      void buildCannonAnimation(Ogre::SceneNode* cannon_animation_node);
*/

   public:
      RenderManager(GameManager* game_manager);
      virtual ~RenderManager();

      void updateAudio();
      void mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);
      void mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);

      void mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y);
      void joystickAxisMoved(int* amount);

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

      void setSelectedNode(std::string item);
      void logComment(std::string comment_message);
      void executeRotateScript(std::string file_name, std::string script_name, std::string object_name, int degrees);

      void setOrientation(SceneNodeMotion* scene_node_motion, double w, double x, double y, double z);
      void setPosition(SceneNodeMotion* scene_node_motion, double x, double y, double z);
      float* getOrientation(SceneNodeMotion* scene_node_motion);
      float* getPosition(SceneNodeMotion* scene_node_motion);

      SceneNodeMotion* createSceneNodeMotion(std::string scene_node_id);
      void applyTorqueImpulse(std::string name, float pitch, float yaw, float roll);
      void stepPhysicsSimulation(float elapsed_time);
      void destroySceneNodeMotion(SceneNodeMotion* snm);

      void drawLine(float* from, float* to, float* color, SceneNodeManual* snm);
      SceneNodeManual* createManualObject();
      void clearManualObject(SceneNodeManual* snm);

      void setGravity(std::string gravity_str);
      std::string networkSendReceive(std::string message_send);
      int getScrollBarSetting();
};

#endif
