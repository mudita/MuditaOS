/*
 * @file GUIWorker.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_GUI_GUIWORKER_HPP_
#define MODULE_SERVICES_SERVICE_GUI_GUIWORKER_HPP_

#include "core/DrawCommand.hpp"
#include "core/Context.hpp"
#include "messages/GUIMessage.hpp"
#include "ServiceGUI.hpp"
#include "Service/Worker.hpp"



namespace sgui {

class ServiceGUI;

enum class GUIWorkerCommands {
	Finish,
	Render
};

class GUIWorkerBufferData {
	std::vector< std::unique_ptr< gui::DrawCommand > > commands;
	gui::Context* context;
};

/*
 *
 */
class GUIWorker: public sys::Worker {
public:
	GUIWorker( ServiceGUI* service );
	virtual ~GUIWorker();

	/**
	 * virtual method responsible for finishing the worker and handling rendering commands
	 */
	bool handleMessage( uint32_t queueID ) override;
};

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_GUIWORKER_HPP_ */
