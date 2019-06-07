/*
 * @file GUIWorker.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_

#include "core/DrawCommand.hpp"
#include "core/Context.hpp"
#include "messages/GUIMessage.hpp"
#include "ServiceGUI.hpp"
#include "Service/Worker.hpp"
#include "gui/core/Renderer.hpp"

namespace sgui {

class ServiceGUI;

enum class WorkerGUICommands {
	Finish,
	Render
};

class WorkerGUIBufferData {
public:
	std::vector<std::unique_ptr<gui::DrawCommand>> commands;
	uint32_t commandsCount;
	gui::Context* context;
};

/*
 *
 */
class WorkerGUI: public sys::Worker {
	//object responsible for rendering images to context
	gui::Renderer renderer;
public:
	WorkerGUI( ServiceGUI* service );
	virtual ~WorkerGUI();

	/**
	 * virtual method responsible for finishing the worker and handling rendering commands
	 */
	bool handleMessage( uint32_t queueID ) override;
};

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_ */
