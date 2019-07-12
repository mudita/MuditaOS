/*
 * @file ViewWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 2 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_
#define MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_

#include "AppWindow.hpp"
#include "widgets/Image.hpp"
namespace gui {

class ViewerState {
public:
	std::string stateName;
	std::string screenName;
	bool deepRefresh = false;
	uint32_t id = 0;
	uint32_t leftState = 0, middleState = 0, rightState = 0;

	ViewerState( std::string stateName, std::string screenName, bool deep, uint32_t id, uint32_t l, uint32_t m, uint32_t r ) :
		stateName{stateName}, screenName{screenName}, deepRefresh{deep}, id{id}, leftState{l}, middleState{m}, rightState{r} {};
};

/*
 *
 */
class ViewWindow: public AppWindow {
	uint32_t currentState = 0;
	std::vector<gui::Image*> images;
	std::vector<ViewerState> states;
public:
	ViewWindow( app::Application* app );
	virtual ~ViewWindow();

	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	bool onInput( const InputEvent& inputEvent ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_ */
