#if !defined GUI_MANAGER
#define GUI_MANAGER

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "tinyxml.h"
#include "CSC2110/TableAVL.h"

class RenderManager;
class GUIWidgetScript;

class GUIManager
{

   private:
      MyGUI::Gui* my_gui;
      MyGUI::OgrePlatform* ogre_platform;
      TableAVL<GUIWidgetScript, std::string>* all_widgets;

      RenderManager* render_manager;

      void addButtons(TiXmlNode* buttons_node, float* value, MyGUI::Window* w);
      void addComboBoxes(TiXmlNode* combo_boxes_node, float* value, MyGUI::Window* w);

   public:
      GUIManager(RenderManager* render_manager);
      virtual ~GUIManager();

      void loadResourceGroup(std::string resource_group_name);
      void unloadResourceGroup();

      void buildGUIFromXML(std::string file_name);

      void mouseMoved(uint32 mouse_x, uint32 mouse_y);
      void mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);
      void mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse);

      void buttonGUIDelegate(MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id);
      void comboGUIDelegate(MyGUI::ComboBox* _sender, uint32 index);

};

#endif
