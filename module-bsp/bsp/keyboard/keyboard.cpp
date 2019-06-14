/*


 * @file keyboard.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details



#include "keyboard.hpp"


#if defined(TARGET_RT1051)

#include "keyboard/rt1051_keyboard.hpp"
#elif defined(TARGET_Linux)

#include "keyboard/bsp_keyboard.hpp"

#else
#error "Unsupported target"
#endif

namespace bsp
{
#if defined(TARGET_RT1051)

    RetCode keyboard::Init(WorkerEvent* worker)
    {
       uint32_t ret =  rt1501_keyboard_Init(worker);
       if(ret != 0)
    	   return RetCode::Failure;
       return RetCode ::Success;
    }

    void keyboard::getData(const uint8_t& notification, KeyState& keyState)
    {
    	keyboard_get_data(notification, keyState);
    }

    RetCode keyboard::DeInit()
    {
        rt1501_keyboard_Deinit();
    }

#elif defined(TARGET_Linux)

    RetCode keyboard::Init(WorkerEvent* worker)
   {
	 return linux_keyboard_Init(worker);
   }

   void keyboard::getData(const uint8_t& notification, KeyState& keyState)
   {

   }

   RetCode keyboard::DeInit()
   {

   }

#else

#error "Unsupported target"
#endif
}
*/
