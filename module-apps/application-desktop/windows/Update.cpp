#include "log/log.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "../data/LockPhoneData.hpp"

// module-utils
#include "i18/i18.hpp"

#include "Update.hpp"
#include "../ApplicationDesktop.hpp"
// services
#include "service-appmgr/ApplicationManager.hpp"

#include "service-cellular/ServiceCellular.hpp"
#include <Style.hpp>

namespace gui
{

    UpdateWindow::UpdateWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_poweroff)
    {
        buildInterface();
    }

    void UpdateWindow::rebuild()
    {
        // find which widget has focus
        uint32_t index = 0;
        for (uint32_t i = 0; i < selectionLabels.size(); i++)
            if (selectionLabels[i] == getFocusItem()) {
                index = i;
                break;
            }

        destroyInterface();
        buildInterface();
        setFocusItem(selectionLabels[index]);
    }
    void UpdateWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        powerImage     = new gui::Image(this, 177, 132, 0, 0, "pin_lock_info");
        powerDownImage = new gui::Image(this, 0, 0, 0, 0, "logo");
        powerDownImage->setVisible(false);

        // title label
        titleLabel = new gui::Label(this, 0, 60, 480, 40);
        titleLabel->setFilled(false);
        titleLabel->setBorderColor(gui::ColorFullBlack);
        titleLabel->setFont(style::header::font::title);
        UTF8 updateText ("Update OS with \"");
        updateText += updateFile.filename().c_str();
        updateText += "\"";
        titleLabel->setText(updateText);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));

        // label with question for powering down
        infoLabel = new gui::Label(this, 0, 294, 480, 30);
        infoLabel->setFilled(false);
        infoLabel->setBorderColor(gui::ColorNoColor);
        infoLabel->setFont(style::window::font::medium);
        infoLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        infoLabel->setText("Do you want to apply this update?");

        uint32_t pinLabelX = 46;
        uint32_t pinLabelY = 350;
        for (uint32_t i = 0; i < 4; i++) {
            gui::Label *label = new gui::Label(this, pinLabelX, pinLabelY, 193, 75);
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);
            label->setPenWidth(0);
            label->setPenFocusWidth(2);
            label->setRadius(5);
            label->setFont(style::window::font::medium);
            label->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            selectionLabels.push_back(label);
            pinLabelX += 193;
        }

        progressBar = new gui::Progress(this, 20, 440, 440, 32);

        selectionLabels[0]->setText(utils::localize.get(style::strings::common::no));
        selectionLabels[1]->setText(utils::localize.get(style::strings::common::yes));

        // define navigation between labels
        selectionLabels[0]->setNavigationItem(NavigationDirection::LEFT, selectionLabels[1]);
        selectionLabels[0]->setNavigationItem(NavigationDirection::RIGHT, selectionLabels[1]);

        selectionLabels[1]->setNavigationItem(NavigationDirection::LEFT, selectionLabels[0]);
        selectionLabels[1]->setNavigationItem(NavigationDirection::RIGHT, selectionLabels[0]);

        // callbacks for getting focus
        selectionLabels[0]->focusChangedCallback = [=](gui::Item &item) {
          if (item.focus)
              this->state = State::Return;
          return true;
        };

        selectionLabels[1]->focusChangedCallback = [=](gui::Item &item) {
          if (item.focus)
              this->state = State::UpdateNow;
          return true;
        };
    }
    void UpdateWindow::destroyInterface()
    {
        erase();
    }

    void UpdateWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_DEBUG("onBeforeShow");
        // on entering screen always set default result as returning to home screen and set focus to "No" label
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
        } else {
            gui::UpdateSwitchData *item = dynamic_cast<gui::UpdateSwitchData *>(data);
            if (item != nullptr) {
                sdesktop::UpdateOsMessage msg = item->getUpdateOsMessage();
                updateFile = msg.updateStats.updateFile;
                progressBar->setTotalProgress(msg.updateStats.totalBytes);
                LOG_DEBUG("totalBytes %lu", msg.updateStats.totalBytes);
                titleLabel->setText(updateFile.filename().c_str());
            }
        }

        state = State::Return;
        setFocusItem(selectionLabels[0]);
    }

    bool UpdateWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        // process only short press, consume rest
        if (inputEvent.state != gui::InputEvent::State::keyReleasedShort)
            return true;

        // if enter was pressed check state and power down or return to main desktop's window
        if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
            if (state == State::Return) {
                application->switchWindow("MainWindow");
            }
            if (state == State::UpdateNow) {
                LOG_DEBUG("onInput sending update now message with file: %s", updateFile.c_str());

                bottomBar->setActive(BottomBar::Side::CENTER, false);
                bottomBar->setActive(BottomBar::Side::RIGHT, false);

                auto msgToSend = std::make_shared<sdesktop::UpdateOsMessage>(updateFile.c_str(), 0);
                sys::Bus::SendBroadcast(msgToSend, application);
                return true;
            }
        }

        return false;
    }

    bool UpdateWindow::handleSwitchData(SwitchData *data)
    {
        gui::UpdateSwitchData *item = dynamic_cast<gui::UpdateSwitchData *>(data);
        if (item != nullptr) {
            progressBar->setCurrentProgress(item->getUpdateOsMessage().updateStats.currentExtractedBytes);

            this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            LOG_DEBUG("currentExtractedBytes %lu", item->getUpdateOsMessage().updateStats.currentExtractedBytes);
        }

        return true;
    }

} /* namespace gui */
