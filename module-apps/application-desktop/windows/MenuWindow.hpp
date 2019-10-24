/*
 * @file MenuWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_

#include <vector>

#include "AppWindow.hpp"
#include "widgets/BottomBar.hpp"
#include "widgets/TopBar.hpp"
#include "widgets/Label.hpp"
#include "widgets/Rect.hpp"

#include "utf8/UTF8.hpp"

namespace gui {

//name of icon, description, name of application to run
struct TileDescription{
	std::string iconName;
	std::string i18Desctiption;
	std::function<bool(Item&)> activatedCallback;
};

class MenuPage : public gui::Rect {
public:
	enum class PageID {
		MainPage= 1,
		ToolsPage
	};
	PageID id;
	uint32_t pageSize = 9;
	gui::Label* title = nullptr;
	std::vector<gui::Item*> tiles;

	MenuPage( gui::Item* parent, int32_t x, int32_t y, uint32_t w, uint32_t h,
	const std::vector<TileDescription>& tilesDescription, MenuPage::PageID id );
	virtual ~MenuPage();

	const PageID& getID(){ return id; };
};

/*
 *
 */
class MenuWindow: public AppWindow {
protected:
	//page that is currently selected by the user.
	uint32_t currentPage =  0;
	std::vector<gui::MenuPage*> pages;
    std::vector<UTF8> page_name;

public:
	MenuWindow( app::Application* app );
	virtual ~MenuWindow();

	void onBeforeShow( ShowMode mode, SwitchData* data ) override;
	bool onInput( const InputEvent& inputEvent ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;

	void switchPage( uint32_t index );
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_ */
