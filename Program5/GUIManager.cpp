#include "GameHeader.h"
#include "GUIManager.h"
#include "RenderManager.h"
#include "GameManager.h"
#include "GUIWidgetScript.h"

#include <iostream>
using namespace std;

int GUIManager::getScrollBarSetting()
{
   MyGUI::ScrollBar* scroll = MyGUI::Gui::getInstance().findWidget<MyGUI::ScrollBar>("Select_Degrees");
   int degrees = -1*(scroll->getScrollRange()/2) + scroll->getScrollPosition();
   return degrees;
}

GUIManager::GUIManager(RenderManager* rm)
{
   render_manager = rm;
   all_widgets = new TableAVL<GUIWidgetScript, std::string >(&GUIWidgetScript::compare_items, &GUIWidgetScript::compare_keys);
}

GUIManager::~GUIManager()
{
   AVLTreeIterator<GUIWidgetScript>* all_widgets_iter = all_widgets->tableIterator();
   while(all_widgets_iter->hasNext())
   {
      GUIWidgetScript* widget = all_widgets_iter->next();
      delete widget;
   }
   delete all_widgets_iter;

   delete all_widgets;
   render_manager = NULL;
}

void GUIManager::unloadResourceGroup()
{
   my_gui->destroyAllChildWidget();

   delete all_widgets;  //contains references to widgets that have been destroyed, so table needs to be cleared out
   all_widgets = new TableAVL<GUIWidgetScript, std::string >(&GUIWidgetScript::compare_items, &GUIWidgetScript::compare_keys);

   MyGUI::ResourceManager& mygui_resource_manager = MyGUI::Singleton<MyGUI::ResourceManager>::getInstance();
   mygui_resource_manager.clear();

   my_gui->shutdown();
   delete my_gui;
   my_gui = NULL;

   ogre_platform->shutdown();
   delete ogre_platform;
   ogre_platform = NULL;
}

void GUIManager::mouseMoved(uint32 mouse_x, uint32 mouse_y)
{
   MyGUI::InputManager& mygui_input_manager = MyGUI::Singleton<MyGUI::InputManager>::getInstance();
   mygui_input_manager.injectMouseMove(mouse_x, mouse_y, 0);
}

//left button is 0, right button is 1
void GUIManager::mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse)
{
   MyGUI::InputManager& mygui_input_manager = MyGUI::Singleton<MyGUI::InputManager>::getInstance();
   mygui_input_manager.injectMousePress(mouse_x, mouse_y, MyGUI::MouseButton::Enum(game_mouse));
}

void GUIManager::mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse)
{
   MyGUI::InputManager& mygui_input_manager = MyGUI::Singleton<MyGUI::InputManager>::getInstance();
   mygui_input_manager.injectMouseRelease(mouse_x, mouse_y, MyGUI::MouseButton::Enum(game_mouse));
}

void GUIManager::loadResourceGroup(std::string resource_group_name)
{
   ogre_platform = new MyGUI::OgrePlatform();
   ogre_platform->initialise(render_manager->getRenderWindow(), render_manager->getSceneManager(), resource_group_name);

   my_gui = new MyGUI::Gui(); 
   my_gui->initialise();

   MyGUI::ResourceManager& mygui_resource_manager = MyGUI::Singleton<MyGUI::ResourceManager>::getInstance();
   mygui_resource_manager.load("MyGUI_Core.xml");

   try
   {
      buildGUIFromXML("gui.xml");
   }
   catch (MyGUI::Exception& e)
   {
      render_manager->logComment(e.what());
      //ASSERT(false);
   }
}

void GUIManager::buttonRotateGUIDelegate(MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id) 
{
   const string& _name = _sender->getName();
   string button_name = _name;

   GUIWidgetScript* widget_script = all_widgets->tableRetrieve(&button_name);
   if (widget_script)
   {
      string script_file_name = widget_script->getFileName();
      string script_function_name = widget_script->getScriptName();

      //obtain a reference to the combo box
      MyGUI::ComboBox* combo = MyGUI::Gui::getInstance().findWidget<MyGUI::ComboBox>("Select_Object");
      int selected_index = combo->getIndexSelected();
      string object_name = combo->getItemNameAt(selected_index);

      MyGUI::ScrollBar* scroll = MyGUI::Gui::getInstance().findWidget<MyGUI::ScrollBar>("Select_Degrees");
      int degrees = -1*(scroll->getScrollRange()/2) + scroll->getScrollPosition();

      render_manager->executeRotateScript(script_file_name, script_function_name, object_name, degrees);
/*
      if (button_name == "Yaw")
      {
         render_manager->applyTorqueImpulse(object_name, 0, degrees, 0);
      }
      else if (button_name == "Pitch")
      {
         render_manager->applyTorqueImpulse(object_name, degrees, 0, 0);
      }
      else if (button_name == "Roll")
      {
         render_manager->applyTorqueImpulse(object_name, 0, 0, degrees);
      }
*/
   }
}

void GUIManager::comboGUIDelegate(MyGUI::ComboBox* _sender, uint32 index) 
{
   const MyGUI::UString item = _sender->getItemNameAt(index);
   //this cast is necessary to make sure that MyGUI classes are not exposed to the render manager
   std::string item_ = (std::string) item;

   cout << item_.c_str() << endl;
   render_manager->setSelectedNode(item_);
}

void GUIManager::buildGUIFromXML(std::string file_name)
{
   float values[4];

   TiXmlDocument doc(file_name.c_str());
   if (doc.LoadFile())
   {
      TiXmlNode* gui_node = doc.FirstChild("gui");
      if (gui_node)
      {
         TiXmlNode* windows_node = gui_node->FirstChild("windows");

         for(TiXmlNode* window_node = windows_node->FirstChild("window"); window_node; window_node = window_node->NextSibling())
         {
            std::string name_text = GameManager::textFromChildNode(window_node, "name");
            std::string caption_text = GameManager::textFromChildNode(window_node, "caption");
            std::string skin_text = GameManager::textFromChildNode(window_node, "skin");
            std::string position_text = GameManager::textFromChildNode(window_node, "position");
            GameManager::parseFloats(position_text, values);
            uint32 left = (uint32) values[0];
            uint32 top = (uint32) values[1];
            std::string size_text = GameManager::textFromChildNode(window_node, "size");
            GameManager::parseFloats(size_text, values);
            uint32 width = (uint32) values[0];
            uint32 height = (uint32) values[1];

            std::string layer_text = GameManager::textFromChildNode(window_node, "layer");

            MyGUI::Window* w = my_gui->createWidget<MyGUI::Window>(skin_text, left, top, width, height, MyGUI::Align::Default, layer_text, name_text);
            w->setCaption(caption_text);

            TiXmlNode* buttons_node = window_node->FirstChild("buttons");

            if (buttons_node)
            {
               addButtons(buttons_node, values, w);
            }

            TiXmlNode* combo_boxes_node = window_node->FirstChild("combo_boxes");

            if (combo_boxes_node)
            {
               addComboBoxes(combo_boxes_node, values, w);
            }

            TiXmlNode* scroll_bars_node = window_node->FirstChild("scroll_bars");

            if (scroll_bars_node)
            {
               addScrollBars(scroll_bars_node, values, w);
            }

         }
      }
   }
   else
   {
      //do nothing
   }
}

void GUIManager::addButtons(TiXmlNode* buttons_node, float* values, MyGUI::Window* w)
{
   for(TiXmlNode* button_node = buttons_node->FirstChild("button"); button_node; button_node = button_node->NextSibling())
   {  
      std::string name_text = GameManager::textFromChildNode(button_node, "name");
      std::string caption_text = GameManager::textFromChildNode(button_node, "caption");
      std::string skin_text = GameManager::textFromChildNode(button_node, "skin");
      std::string position_text = GameManager::textFromChildNode(button_node, "position");
      GameManager::parseFloats(position_text, values);
      uint32 left = (uint32) values[0];
      uint32 top = (uint32) values[1];
      std::string size_text = GameManager::textFromChildNode(button_node, "size");
      GameManager::parseFloats(size_text, values);
      uint32 width = (uint32) values[0];
      uint32 height = (uint32) values[1];

      std::string file_name_text = GameManager::textFromChildNode(button_node, "file_name");
      std::string script_name_text = GameManager::textFromChildNode(button_node, "script_name");

      std::string font_size_text = GameManager::textFromChildNode(button_node, "font");
      uint32 font_size = (uint32) GameManager::parseFloat(font_size_text);

      MyGUI::Button* b = w->createWidget<MyGUI::Button>(skin_text, left, top, width, height, MyGUI::Align::Default, name_text);
      b->setCaption(caption_text);
      b->setFontHeight(font_size);
      b->setTextColour(MyGUI::Colour(0,0,0));
      b->eventMouseButtonPressed += newDelegate(this, &GUIManager::buttonRotateGUIDelegate);

      GUIWidgetScript* widget_script = new GUIWidgetScript(b, name_text);
      widget_script->setFileName(file_name_text);
      widget_script->setScriptName(script_name_text);

      all_widgets->tableInsert(widget_script);
   }
}

void GUIManager::addComboBoxes(TiXmlNode* combo_boxes_node, float* values, MyGUI::Window* w)
{
   for(TiXmlNode* combo_box_node = combo_boxes_node->FirstChild("combo_box"); combo_box_node; combo_box_node = combo_box_node->NextSibling())
   {
      std::string name_text = GameManager::textFromChildNode(combo_box_node, "name");
      std::string skin_text = GameManager::textFromChildNode(combo_box_node, "skin");
      std::string position_text = GameManager::textFromChildNode(combo_box_node, "position");
      GameManager::parseFloats(position_text, values);
      uint32 left = (uint32) values[0];
      uint32 top = (uint32) values[1];

      std::string size_text = GameManager::textFromChildNode(combo_box_node, "size");
      GameManager::parseFloats(size_text, values);
      uint32 width = (uint32) values[0];
      uint32 height = (uint32) values[1];

      std::string align_text = GameManager::textFromChildNode(combo_box_node, "align");
      std::string font_size_text = GameManager::textFromChildNode(combo_box_node, "font");
      uint32 font_size = (uint32) GameManager::parseFloat(font_size_text);

      std::string selected_index_text = GameManager::textFromChildNode(combo_box_node, "selected_index");
      int selected_index = (uint32) GameManager::parseFloat(selected_index_text);

      MyGUI::ComboBox* combo = w->createWidget<MyGUI::ComboBox>(skin_text, left, top, width, height, MyGUI::Align::Default, name_text);
      combo->setFontHeight(font_size);
      combo->setTextColour(MyGUI::Colour(0,0,0));

      TiXmlNode* selections_node = combo_box_node->FirstChild("selections");
      for(TiXmlNode* selection_node = selections_node->FirstChild("selection"); selection_node; selection_node = selection_node->NextSibling())
      {
         TiXmlElement* selection_element = (TiXmlElement*) selection_node->ToElement();
         std::string selection_text = selection_element->GetText();
         combo->addItem(selection_text);
      }

      combo->setIndexSelected(selected_index);
      combo->setComboModeDrop(true);
      combo->eventComboAccept += newDelegate(this, &GUIManager::comboGUIDelegate);
   }
}

void GUIManager::addScrollBars(TiXmlNode* scroll_bars_node, float* values, MyGUI::Window* w)
{
   for(TiXmlNode* scroll_bar_node = scroll_bars_node->FirstChild("scroll_bar"); scroll_bar_node; scroll_bar_node = scroll_bar_node->NextSibling())
   {
      std::string name_text = GameManager::textFromChildNode(scroll_bar_node, "name");
      std::string skin_text = GameManager::textFromChildNode(scroll_bar_node, "skin");
      std::string position_text = GameManager::textFromChildNode(scroll_bar_node, "position");
      GameManager::parseFloats(position_text, values);
      uint32 left = (uint32) values[0];
      uint32 top = (uint32) values[1];

      std::string size_text = GameManager::textFromChildNode(scroll_bar_node, "size");
      GameManager::parseFloats(size_text, values);
      uint32 width = (uint32) values[0];
      uint32 height = (uint32) values[1];

      std::string align_text = GameManager::textFromChildNode(scroll_bar_node, "align");

      std::string track_text = GameManager::textFromChildNode(scroll_bar_node, "track_size");
      int track_size = GameManager::parseInt(track_text);

      std::string range_text = GameManager::textFromChildNode(scroll_bar_node, "scroll_range");
      int scroll_range = GameManager::parseInt(range_text);

      std::string scroll_position_text = GameManager::textFromChildNode(scroll_bar_node, "scroll_position");
      int scroll_position = GameManager::parseInt(scroll_position_text);

      std::string vertical_text = GameManager::textFromChildNode(scroll_bar_node, "vertical");
      bool vertical = (vertical_text == "yes");

      //look at MyGUI_BlueWhiteTemplates.xml to find the correct skin
      MyGUI::ScrollBar* scroll = w->createWidget<MyGUI::ScrollBar>(skin_text, left, top, width, height, MyGUI::Align::Default, name_text);

      scroll->setTrackSize(track_size);
      scroll->setScrollRange(scroll_range);
      scroll->setScrollPosition(scroll_position);
      scroll->setVerticalAlignment(vertical);
   }
}
