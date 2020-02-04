/*
 * @file MenuWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "service-appmgr/ApplicationManager.hpp"
#include "../ApplicationDesktop.hpp"
#include "MenuWindow.hpp"
#include "Navigation.hpp"
#include "Item.hpp"

#include "i18/i18.hpp"
#include <Style.hpp>

namespace gui {


MenuPage::MenuPage( gui::Item* parent, int32_t x, int32_t y, uint32_t w, uint32_t h,
		const std::vector<TileDescription>& tilesDescription,
		MenuPage::PageID id ) :
	Rect( parent, x, y, w, h ),
	id{ id }{

	setBorderColor( ColorNoColor );
	setFilled( false );

	uint32_t rowCount = tilesDescription.size()/3;

	for( uint32_t i=0; (i<9 && i<tilesDescription.size()); i++) {

		const TileDescription& tileString = tilesDescription[i];
		uint32_t pageRow = (i%pageSize) / 3;
		uint32_t pageColumn = (i%pageSize) % 3;
		gui::Rect* tile = new gui::Rect( this, 31 + pageColumn*(128 + 17 ), 60+pageRow*(128+17), 128, 128 );
		tile->setRadius(11);
		tile->setPenFocusWidth(2);
		tile->setPenWidth(0);

		gui::Image* tileImg = new gui::Image( tile, 0, 0, 0,0, tileString.iconName );
		tileImg->setPosition( (tile->widgetArea.w - tileImg->widgetArea.w)/2, (tile->widgetArea.h - tileImg->widgetArea.h)/3 );

		gui::Label* titleDesc = new gui::Label( tile, 0, 0, tile->widgetArea.w, 103);
		titleDesc->setBorderColor( gui::ColorNoColor );
		titleDesc->setFont(style::window::font::small);
		titleDesc->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		titleDesc->setText( utils::localize.get(tileString.i18Desctiption) );

		//set activation callback
		tile->activatedCallback = tilesDescription[i].activatedCallback;

		tiles.push_back( tile );
	}

	//define navigation between items
	for( uint32_t i=0; i<rowCount; i++ ) {
		for( uint32_t j=0; j<3; j++ ) {
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::RIGHT, tiles[i*3+ (j+1)%3] );
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::LEFT, tiles[i*3+ (j+2)%3] );
		}
	}

	//define navigation between items
	if( tilesDescription.size() > 3 ) {
		for( uint32_t i=0; i<rowCount; i++ ) {
			for( uint32_t j=0; j<3; j++ ) {
				tiles[i*3 + j]->setNavigationItem( NavigationDirection::UP, tiles[ ((i+2)%3*3) + j] );
				tiles[i*3 + j]->setNavigationItem( NavigationDirection::DOWN, tiles[ ((i+1)*3)%9 + j] );
			}
		}
	}
	setVisible(false);

	//set focus gain callback. When page receives focus it gives it to the first tile.
	focusChangedCallback = [=] (gui::Item& item){
		setVisible(true);
		return true;
	};
}

MenuPage::~MenuPage() {

}

MenuWindow::MenuWindow( app::Application* app ) : AppWindow(app,"MenuWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void MenuWindow::rebuild() {
	//find which widget has focus
	uint32_t index = 0;
	MenuPage* page = reinterpret_cast<MenuPage*>(pages[currentPage]);
	for( uint32_t j=0; j<page->tiles.size(); j++ ) {
		if( page->tiles[j] == getFocusItem()) {
			index = j;
			break;
		}
	}
	focusItem = nullptr;
	destroyInterface();
	buildInterface();
	page = reinterpret_cast<MenuPage*>(pages[currentPage]);
	pages[currentPage]->setVisible(true);
	setFocusItem( page->tiles[index] );
}

void MenuWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::SIGNAL, true  );
	topBar->setActive(TopBar::Elements::BATTERY, true );

    std::vector<TileDescription> page1Definitions{
        // page1
        TileDescription{"menu_phone", "app_desktop_menu_phone",
                        [=](gui::Item &item) {
                            LOG_INFO("Call Log");
                            sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationCallLog", gui::name::window::main_window, nullptr);
                            return true;
                        }},
        TileDescription{"menu_contacts", "app_desktop_menu_contacts",
                        [=](gui::Item &item) {
                            LOG_INFO("Phonebook");
                            sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationPhonebook", gui::name::window::main_window, nullptr);
                            return true;
                        }},
        TileDescription{"menu_messages", "app_desktop_menu_messages",
                        [=](gui::Item &item) {
                            LOG_INFO("Messages");
                            sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationMessages", gui::name::window::main_window, nullptr);
                            return true;
                        }},
        TileDescription{"menu_calendar", "app_desktop_menu_calendar", [=](gui::Item &item) { return true; }},
        TileDescription{"menu_alarm", "app_desktop_menu_alarm", [=](gui::Item &item) { return true; }},
        TileDescription{"menu_meditation", "app_desktop_menu_meditation", [=](gui::Item &item) { return true; }},
        TileDescription{"menu_music_player", "app_desktop_menu_music", [=](gui::Item &item) { return true; }},
        TileDescription{
            "menu_tools",
            "app_desktop_menu_tools",
            [=](gui::Item &item) {
                {
                    LOG_INFO("page 1 tools");
                    switchPage(1);
                    return true;
                }
            },
        },
        TileDescription{"menu_settings", "app_desktop_menu_settings",
                        [=](gui::Item &item) {
                            LOG_INFO("page 1 settings");
                            sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationSettings", gui::name::window::main_window, nullptr);
                            return true;
                        }},
    };

    MenuPage* page1 = new MenuPage( this, 0, 60, 480, 70+128*3+2*17, page1Definitions, MenuPage::PageID::MainPage);
	pages.push_back(page1);
    page_name.push_back(utils::localize.get("app_desktop_menu_title"));

	//PAGE 2
    std::vector<TileDescription> page2Definitions{
        TileDescription{"menu_tools_notes", "app_desktop_tools_notes",
                        [=](gui::Item &item) {
                            sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationNotes", gui::name::window::main_window, nullptr);
                            return true;
                        }},
        TileDescription{"menu_tools_calculator", "app_desktop_tools_calculator", [=](gui::Item &item) { return true; }},
        TileDescription{"menu_tools_recorder", "app_desktop_tools_recorder", [=](gui::Item &item) { return true; }},
    };

    MenuPage* page2 = new MenuPage( this, 0, 60, 480, 70+128*3+2*17, page2Definitions, MenuPage::PageID::ToolsPage );
	pages.push_back( page2 );
    page_name.push_back(utils::localize.get("app_desktop_tools_title"));

    setTitle(page_name[currentPage]);
}
void MenuWindow::destroyInterface() {
	AppWindow::destroyInterface();
	for( MenuPage* mp : pages )
		delete mp;
	pages.clear();
	focusItem = nullptr;
	children.clear();
}

MenuWindow::~MenuWindow() {
	destroyInterface();
}

void MenuWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {

	//select middle row and middle column to assign focus to the element.
	Item* item = pages[currentPage]->tiles[0];
	setFocusItem(item);
	pages[currentPage]->setVisible(true);
}

bool MenuWindow::onInput( const InputEvent& inputEvent ) {
	
	if(( inputEvent.state == InputEvent::State::keyReleasedShort ) || ( inputEvent.state == InputEvent::State::keyReleasedLong )) {
		// putsh enter on tile number on key press (press 9 to enter 9th tile)
        size_t num = -1;
        auto key = gui::toNumeric(inputEvent.keyCode);
        if (std::isdigit(key))
        {
            std::stringstream str;
            str << gui::toNumeric(inputEvent.keyCode);
            str >> num;
            num -= 1;
        }
        if ((num >= 0) && num < (pages[currentPage]->tiles.size()))
        {
            LOG_ERROR("RUN tile with enter -> %d %d", num,pages[currentPage]->tiles.size());
			auto el = Item();
			return pages[currentPage]->tiles[num]->activatedCallback(el);
        }
        switch( inputEvent.keyCode ) {
			case KeyCode::KEY_ENTER:
				LOG_INFO("Enter pressed");
				break;
			case KeyCode::KEY_RF: {
				if( pages[currentPage]->getID() == MenuPage::PageID::ToolsPage ) {
					switchPage(0);
				}
				else {
                    application->switchWindow(gui::name::window::main_window);
                }
				return true;
			}
			default:
				break; 
		}	
	} 
	
	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

void MenuWindow::switchPage( uint32_t index ) {
	//check if index hase valid value
	if( index >= pages.size()) return;

	//set visibility of all pages to false
	pages[currentPage]->setVisible(false);
	currentPage = index;
	pages[currentPage]->setVisible(true);
	//give focus to element
	Item* item = pages[currentPage]->tiles[0];
	setFocusItem(item);
    setTitle(page_name[currentPage]);
	application->refreshWindow( RefreshModes::GUI_REFRESH_DEEP );
}

} /* namespace gui */
