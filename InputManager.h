#if !defined INPUT_MANAGER
#define INPUT_MANAGER

#include <OIS.h>
#include "CSC2110/ListArray.h"
#include "InputListener.h"
#include <iostream>

class GameManager;

class InputManager: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
   private:
      OIS::InputManager* input_manager_ois;

      OIS::Keyboard* keyboard_ois;
      OIS::Mouse* mouse_ois;
      OIS::JoyStick* joystick_ois;

      int window_width;
      int window_height;

      ListArray<InputListener>* input_listeners;
      
      std::string keyMap(const OIS::KeyEvent& e);
      uint32 mouseMap(const OIS::MouseButtonID id);
      std::string joystickButtonMap(int button);
      std::string joystickAxisMap(int axis);

      void init(GameManager* game_manager);
      void free();

   public:
      InputManager(GameManager* game_manager);
      virtual ~InputManager();

      void addListener(InputListener* listener);
      void checkForInput(float time_step);

      //key listener methods
      bool keyPressed(const OIS::KeyEvent& e);
      bool keyReleased(const OIS::KeyEvent& e);

      //mouse listener methods
      bool mouseMoved(const OIS::MouseEvent& e);
      bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
      bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

      //joystick listener methods
      bool buttonPressed (const OIS::JoyStickEvent &arg, int button);
      bool buttonReleased (const OIS::JoyStickEvent &arg, int button);
      bool axisMoved (const OIS::JoyStickEvent &arg, int axis);
      //not common
      bool sliderMoved (const OIS::JoyStickEvent &, int index);
      bool povMoved (const OIS::JoyStickEvent &arg, int index);
      bool vector3Moved (const OIS::JoyStickEvent &arg, int index);
};

#endif
