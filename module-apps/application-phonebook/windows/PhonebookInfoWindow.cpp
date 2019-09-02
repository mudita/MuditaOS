#include "PhonebookInfoWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"

#include <log/log.hpp>

#include "PhonebookData.hpp"

namespace gui {

PhonebookInfo::PhonebookInfo(app::Application *app) : AppWindow(app, "PhonebookInfo"), text_label(nullptr), image(nullptr), box(nullptr) {
    setSize(default_width, default_height);
    buildInterface();
}

void PhonebookInfo::rebuild() {
    destroyInterface();
    buildInterface();
}

void PhonebookInfo::buildInterface() {
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    gui::Label *title = new gui::Label(this, 0, 0, default_width, 50, "");
    title->setMaxSize(default_width, 70);
    title->setFilled(false);
    title->setBorderColor(gui::ColorNoColor);
    title->setFont("gt_pressura_bold_24");
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    title->setText(utils::localize.get("app_phonebook_info_title"));

    if (!image) {
        image = new gui::Image(this, 177, 132, 0, 0, "pin_lock_info");
    }
    if (!text_label) {
        text_label = new gui::Label(this, 0, image->widgetArea.y + image->widgetArea.h + 50, default_width, 50);
        text_label->setFilled(false);
        text_label->setBorderColor(gui::ColorNoColor);
        text_label->setFont("gt_pressura_regular_24");
        text_label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    }
}

void PhonebookInfo::destroyInterface() {
    focusItem = nullptr;
    AppWindow::destroyInterface();
    children.clear();
    text_label = nullptr;
    image = nullptr;
    box = nullptr;
}

PhonebookInfo::~PhonebookInfo() { destroyInterface(); }

void PhonebookInfo::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
    if (this->data != nullptr && text_label != nullptr) {
        UTF8 show_me = "";
        int placeholderlen = strlen("###");
        int32_t val = 0, prew_val = 0;
        for (auto text : this->data->data) {
            if ((val = this->data->format_str.find("###", val) + placeholderlen) != UTF8::npos) {
                show_me += this->data->format_str.substr(prew_val, val - prew_val - placeholderlen);
                show_me += text;
                prew_val = val;
            } else {
                break;
            }
        }
        static const int offset = 10;
        show_me += this->data->format_str.substr(prew_val, this->data->format_str.length() - prew_val);
        if (box != nullptr) {
            focusItem = nullptr;
            this->removeWidget(box);
            delete box;
            box = nullptr;
        }
        box = new gui::HBox(this, offset, text_label->widgetArea.y + text_label->widgetArea.h, default_width - 2 * offset, 70);
        for (auto option : this->data->options) {
            auto label = new gui::Label(box, 0, 0, box->widgetArea.w / this->data->options.size() - 1 - offset * 2, 70, option.first);
            label->setFont("gt_pressura_bold_24");
            label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            label->activatedCallback = option.second;
            label->setRadius(11);
            label->setPenFocusWidth(3);
            label->setPenWidth(1);
        }
        for (auto child = box->children.begin(); child != box->children.end(); ++child) {
            if (child == box->children.begin()) {
                setFocusItem(*child);
            }
            if (box->children.end() != std::next(child, 1)) {
                (*child)->setNavigationItem(NavigationDirection::RIGHT, *std::next(child, 1));
            }
            if (box->children.begin() != child) {
                (*child)->setNavigationItem(NavigationDirection::LEFT, *std::prev(child, 1));
            }
        }
        if (box) {
            box->resizeItems();
            box->setBorderColor(gui::ColorNoColor);
        }
        text_label->setText(show_me);
    }
}

bool PhonebookInfo::handleSwitchData(SwitchData *data) {
    if (data != nullptr) {
        this->data = std::make_unique<PhonebookInfoData>(reinterpret_cast<PhonebookInfoData *>(data));
    };
    return true;
}

bool PhonebookInfo::onInput(const InputEvent &inputEvent) {
    // check if any of the lower inheritance onInput methods catch the event
    bool ret = AppWindow::onInput(inputEvent);
    if (ret) {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
            application->render(RefreshModes::GUI_REFRESH_FAST);
        return true;
    }

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_RF) {
        application->switchWindow("MainWindow", 0, std::move(data));
        return true;
    }

    return false;
}

} /* namespace gui */
