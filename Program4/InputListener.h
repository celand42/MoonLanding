#if !defined INPUT_LISTENER
#define INPUT_LISTENER

#include <string>
#include "GameHeader.h"

class InputListener
{
   private:

   public:
      virtual ~InputListener(){};

      virtual void keyPressed(std::string game_key) = 0;
      virtual void keyReleased(std::string game_key) = 0;

      virtual void mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y) = 0;
      virtual void mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse) = 0;
      virtual void mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse) = 0;

      //joystick methods
      virtual void joystickButtonPressed(std::string button) = 0;
      virtual void joystickAxisMoved(int* amount) = 0;
};

#endif