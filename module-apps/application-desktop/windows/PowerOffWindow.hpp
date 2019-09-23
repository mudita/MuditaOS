/*
 * @file PowerOff.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_POWEROFFWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_POWEROFFWINDOW_HPP_

#include <vector>
#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui {

/*
 * Class that defines look and functions for power down window.
 */
class PowerOffWindow: public AppWindow {

enum class State {
		PowerDown,
		Return,
	};

	gui::Label* titleLabel = nullptr;
	gui::Label* infoLabel = nullptr;

	std::vector<gui::Label*> selectionLabels;
	//TODO remove this label after power manager is finished.
	gui::Label* eventMgrLabel = nullptr;
	gui::Image* powerImage = nullptr;
	gui::Image* powerDownImage = nullptr;
	State state = State::Return;

public:
	PowerOffWindow( app::Application* app );
	virtual ~PowerOffWindow();
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	bool onInput( const InputEvent& inputEvent ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;

};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_POWEROFFWINDOW_HPP_ */
