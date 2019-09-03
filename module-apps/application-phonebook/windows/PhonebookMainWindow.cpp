#include "PhonebookMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "PhonebookData.hpp"
#include <log/log.hpp>
#include "DefaultStyle.hpp"

namespace gui {

PhonebookMainWindow::PhonebookMainWindow(app::Application *app)
    : AppWindow(app, "MainWindow"), contact_size_h_px(style::default_label_h),window_margin(20),topbox(nullptr), left(nullptr), right(nullptr)
{
    setSize(default_width, default_height);

    model = new ContactListModel(app, 7);
    controller = new ContactListController(app, model, this);

    buildInterface();
}

void PhonebookMainWindow::rebuild() {
    // find which widget has focus
    destroyInterface();
    buildInterface();
}

void PhonebookMainWindow::buildInterface() {
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    if(!topbox) {
        static const int arrow_w = 16;
        topbox = new HBox(this, 0, style::default_label_h, default_width, style::default_label_h);
        topbox->setFilled(false);
        topbox->setBorderColor(gui::ColorNoColor);
        left = new gui::Image(topbox, 0, 0, style::default_label_h, arrow_w, "left_label_arrow");
        left->activatedCallback = [=](Item&) { return application->switchWindow("NewContact",0,nullptr); };
        auto title = new gui::Label(topbox, 0, 0, default_width-2*arrow_w-1, style::default_label_h);
        title->setMaxSize(default_width-arrow_w*2-1, style::default_label_h);
        title->setFilled(false);
        title->setBorderColor(gui::ColorNoColor);
        title->setFont("gt_pressura_bold_24");
        title->setText(utils::localize.get("app_phonebook_title_main"));
        title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        right = new gui::Image(topbox, 0, 0, style::default_label_h, arrow_w, "right_label_arrow");
        right->activatedCallback = [=](Item&) { addSearch(); return true; };
        topbox->resizeItems();
    }

    controller->setViewArea(BoundingBox(0,topbox->offset_h(), this->w(), usable_h()));
    controller->initialize();
    setFocusItem(controller);
}

void PhonebookMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    topbox = nullptr;
    this->focusItem = nullptr;
    children.clear();
}

PhonebookMainWindow::~PhonebookMainWindow() { destroyInterface(); }

void PhonebookMainWindow::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
}

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent) {
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

    if(left && inputEvent.keyCode == KeyCode::KEY_LEFT) {
        left->onActivated(nullptr);
    } else if(right && inputEvent.keyCode == KeyCode::KEY_RIGHT) {
        right->onActivated(nullptr);
    }

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

uint32_t PhonebookMainWindow::usable_h()
{
    return this->h()-topBar->h()-bottomBar->h()-topbox->h();
}

UTF8 utf_deserialize_to_utf(uint32_t val) {
    union {
        char el[5];
        uint32_t val;
    }letter_store;
    memset(&letter_store, 0, sizeof letter_store);
    letter_store.val=val;
    return UTF8(letter_store.el);
}

void PhonebookMainWindow::addSearch()
{
}
} /* namespace gui */
