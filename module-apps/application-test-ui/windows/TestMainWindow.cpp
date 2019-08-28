#include "TestMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationTest.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "TestMainWindow.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>

namespace gui {

TestMainWindow::TestMainWindow(app::Application *app) : AppWindow(app, "MainWindow") {
    setSize(480, 600);
    buildInterface();
}

void TestMainWindow::rebuild() {
    destroyInterface();
    buildInterface();
    setFocusItem(show_box[0]);
}

void TestMainWindow::buildInterface() {
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    title = new gui::Label(this, 0, 50, 480, 50);
    title->setFilled(false);
    title->setBorderColor(gui::ColorNoColor);
    title->setFont("gt_pressura_bold_24");
    title->setText("GUI TEST APP");
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

    static const int pos_x1 =(480 - 200)/2, pos_y1 = 200;
    static const int size_x =100, size_y = 100;

    enum {
        LeftUp = 0,
        RightUp,
        LeftDown,
        RightDown,
    };

    // assign new elements from left to right in rows
    show_box.push_back( new gui::Label(this,pos_x1 , pos_y1, size_x, size_y));
    show_box.push_back( new gui::Label(this,pos_x1 + size_x + 10 , pos_y1, size_x, size_y));
    show_box.push_back( new gui::Label(this,pos_x1 , pos_y1 + size_y + 10, size_x, size_y));
    show_box.push_back( new gui::Label(this,pos_x1 + size_x + 10, pos_y1 + size_y + 10, size_x, size_y));

    // se texts
    bottom_texts = {
        "MESSAGE", "CALL",
    };

    std::vector<UTF8> images = {
        "menu_messages", "menu_phone"
    };

    int i = 0;
    for ( auto el : show_box) {
        // for left elements set border 2 for right border 3
        int el_num = (i==0||i==3)?0:1;
        el->setPenFocusWidth(i%2?3:2);
        el->setPenWidth(0);
        el->setRadius(11);
        el->setText(std::to_string(i));
        auto al = Alignment();
        al.setHorizontal(Alignment::ALIGN_HORIZONTAL_CENTER);
        al.setVertical(Alignment::ALIGN_VERTICAL_TOP);
        el->setAlignement(al);
        el->focusChangedCallback = [=](Item &it) {
            if(it.focus) {
                this->bottomBar->setText(BottomBar::Side::CENTER, bottom_texts[el_num]);
            }
            return true;
        };
        auto image = new gui::Image( el, 0, 0, 0,0, images[el_num]);
        image->setPosition((el->widgetArea.w-image->widgetArea.w)/2, (el->widgetArea.h-image->widgetArea.h)/2);
        el->activatedCallback = [=](Item&) {
            static bool change = true;
            this->bottomBar->setText(BottomBar::Side::CENTER, bottom_texts[change?!el_num:el_num]);
            application->render(RefreshModes::GUI_REFRESH_FAST);
            change = !change;
            return true;
        };
        ++i;
    }

    // to right
    show_box[LeftUp]->setNavigationItem(NavigationDirection::RIGHT, show_box[RightUp]);
    show_box[LeftDown]->setNavigationItem(NavigationDirection::RIGHT, show_box[RightDown]);

    // to left
    show_box[RightUp]->setNavigationItem(NavigationDirection::LEFT, show_box[LeftUp]);
    show_box[RightDown]->setNavigationItem(NavigationDirection::LEFT, show_box[LeftDown]);

    // to up
    show_box[RightDown]->setNavigationItem(NavigationDirection::UP, show_box[RightUp]);
    show_box[LeftDown]->setNavigationItem(NavigationDirection::UP, show_box[LeftUp]);

    // to down
    show_box[RightUp]->setNavigationItem(NavigationDirection::DOWN, show_box[RightDown]);
    show_box[LeftUp]->setNavigationItem(NavigationDirection::DOWN, show_box[LeftDown]);

    /// set on focus text bottom change

    setFocus(show_box[0]);
}

void TestMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    delete title;
    this->focusItem = nullptr;
    children.clear();
}

TestMainWindow::~TestMainWindow() { destroyInterface(); }

gui::Item *TestMainWindow::addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback) {
    gui::Label *label = new gui::Label(this, 17, 0, 480 - 34, 60, text);
    label->setMargins(gui::Margins(16, 0, 0, 0));
    label->setFilled(false);
    label->setPenFocusWidth(3);
    label->setPenWidth(0);
    label->setFont("gt_pressura_regular_24");
    label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    label->setRadius(11);
    label->activatedCallback = activatedCallback;

    return label;
}

void TestMainWindow::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) { setFocusItem(show_box[0]); }

bool TestMainWindow::onInput(const InputEvent &inputEvent) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER )
			application->render( RefreshModes::GUI_REFRESH_FAST );
		return true;
	}

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return false;

    if( inputEvent.keyCode == KeyCode::KEY_1) {
        LOG_INFO("Pressed 1");
        application->switchWindow("NewContact",0,nullptr);
    }
	if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Enter pressed");
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationDesktop", "MenuWindow", nullptr );
		return true;
	}

	return false;

}

} /* namespace gui */
