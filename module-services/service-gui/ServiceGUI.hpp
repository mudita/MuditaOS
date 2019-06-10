/*
 * ServiceGUI.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_

//module-gui
#include "gui/core/Context.hpp"
#include "gui/core/Renderer.hpp"

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "GUIWorker.hpp"

class GUIWorker;

namespace sgui {

class ServiceGUI: public sys::Service {
protected:
	//this is where every incomming frame is painted.
	gui::Context* renderContext;
	//this buffer is provided to eink
	gui::Context* transferContext;
	//ID of the last rendered frame
	uint32_t renderFrameCounter;
	//ID of the last frame sent to eink for rendering
	uint32_t transferedFrameCounter;
	//Horizontal size of the screen in pixels
	uint32_t screenWidth;
	//vertical size of the screen in pixels
	uint32_t screenHeight;
	//object responsible for rendering images to context
	gui::Renderer renderer;
	//flag that defines whether eink is ready for new frame buffer
	volatile bool einkReady = false;
	volatile bool requestSent = false;
	volatile bool bufferLocked = false;
	uint32_t timer_id= 0;
	gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_DEEP;

//	GUIWorker* worker;

	void sendBuffer();

public:
    ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight );
    ~ServiceGUI();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;
};

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_ */
