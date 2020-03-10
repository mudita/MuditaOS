/*
 * @file ApplicationDesktop.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 18 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"

namespace app {

    namespace name::window
    {
        inline const std::string desktop_menu = "MenuWindow";
        inline const std::string desktop_lockscreen = gui::name::window::main_window;
    }; // namespace name::window

inline const std::string name_desktop = "ApplicationDesktop";

class ApplicationDesktop : public Application {
protected:
	//determines whether screen should be protected by pin verification
	bool screenLocked = true;
	bool pinLocked = false;
public:
  bool need_sim_select = false;
  struct Notifications
  {
      unsigned int notSeenSMS = 0;
      unsigned int notSeenCalls = 0;
  } notifications;
    ApplicationDesktop( std::string name=name_desktop, std::string parent = "", bool startBackground =false );
	virtual ~ApplicationDesktop();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

    void createUserInterface() override;
    void destroyUserInterface() override;
    bool getScreenLocked();
	void setScreenLocked( bool val );
	bool getPinLocked();
	/**
	 * This static method will be used to lock the phone
	 */
//	static bool messageLockPhone( sys::Service* sender, std::string application , const gui::InputEvent& event );
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_ */
