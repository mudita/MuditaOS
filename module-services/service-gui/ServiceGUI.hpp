/*
 * ServiceGUI.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_

#include <memory>

//module-gui
#include "gui/core/Context.hpp"
#include "gui/core/Renderer.hpp"
#include "messages/DrawMessage.hpp"

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "WorkerGUI.hpp"

namespace sgui {

class WorkerGUI;

class ServiceGUI: public sys::Service {
	friend WorkerGUI;
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
	volatile bool rendering = false;
	//set of commands recently received. If this vector is not empty and new set of commands is received
	//previous commands are removed.
	std::vector<std::unique_ptr<gui::DrawCommand>> commands;
//	uint32_t timer_id= 0;
	gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_DEEP;

	//semaphore used to protect commands vector while commands are taken from service to worker.
	SemaphoreHandle_t semCommands;

	WorkerGUI* worker;

	void sendBuffer();
	void sendToRender();

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
