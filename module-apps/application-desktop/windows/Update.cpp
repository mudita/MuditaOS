#include "log/log.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "../data/LockPhoneData.hpp"
#include <source/version.hpp>

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

        // questionImage = new gui::Image(this, 177, 132, 0, 0, "pin_lock_info");

        // title label
        titleLabel = new gui::Label(this, 0, 60, 480, 40);
        titleLabel->setFilled(false);
        titleLabel->setBorderColor(gui::ColorFullBlack);
        titleLabel->setFont(style::header::font::title);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        titleLabel->setText("Apply update?");

        // Update version information
        updateVersionInfo = new gui::Label(this, 10, 132, 480, 40);
        updateVersionInfo->setFilled(false);
        updateVersionInfo->setBorderColor(gui::ColorFullBlack);
        updateVersionInfo->setFont(style::window::font::smallbold);
        updateVersionInfo->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        updateVersionInfo->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        updateVersionInfo->setText("Update: ");

        // Update details
        updateDetails = new gui::Label(this, 40, 172, 440, 40);
        updateDetails->setFilled(false);
        updateDetails->setBorderColor(gui::ColorFullBlack);
        updateDetails->setFont(style::window::font::verysmall);
        updateDetails->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        updateDetails->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        updateDetails->setText("Update: ");

        // Current version information
        currentVersionInfo = new gui::Label(this, 10, 222, 480, 40);
        currentVersionInfo->setFilled(false);
        currentVersionInfo->setBorderColor(gui::ColorFullBlack);
        currentVersionInfo->setFont(style::window::font::small);
        currentVersionInfo->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        currentVersionInfo->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        currentVersionInfo->setText("Current: ");

        // Label Info
        infoLabel = new gui::Label(this, 20, 304, 440, 40);
        infoLabel->setFilled(false);
        infoLabel->setBorderColor(gui::ColorNoColor);
        infoLabel->setFont(style::window::font::medium);
        infoLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        infoLabel->setText("Do you want to apply this update?");

        // Details during update
        detailLabel = new gui::Label(this, 20, 354, 440, 20);
        detailLabel->setFilled(false);
        detailLabel->setBorderColor(gui::ColorNoColor);
        detailLabel->setFont(style::window::font::small);
        detailLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Bottom));
        detailLabel->setText("");

        // update progress
        percentLabel = new gui::Label(this, 0, 374, 520, 128);
        percentLabel->setFilled(false);
        percentLabel->setBorderColor(gui::ColorNoColor);
        percentLabel->setFont(style::window::font::largelight);
        percentLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        percentLabel->setVisible(false);

        uint32_t pinLabelX = 46;
        uint32_t pinLabelY = 410;
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
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
        } else {
            gui::UpdateSwitchData *item = dynamic_cast<gui::UpdateSwitchData *>(data);
            if (item != nullptr) {
                std::stringstream currentVersion;
                std::stringstream updateVersion;
                std::stringstream updateFileDetails;

                sdesktop::UpdateOsMessage msg = item->getUpdateOsMessage();
                updateFile                    = msg.updateStats.updateFile;
                currentVersion << "Current: ";
                currentVersion << VERSION;
                currentVersion << " (";
                currentVersion << GIT_REV;
                currentVersion << ")";

                updateVersion << "Update to: ";
                updateVersion << msg.updateStats
                                     .versioInformation[purefs::json::os_version][purefs::json::version_string]
                                     .string_value();
                updateVersion << " (";
                updateVersion << msg.updateStats
                                     .versioInformation[purefs::json::git_info][purefs::json::os_git_revision]
                                     .string_value();
                updateVersion << ")";

                vfs::FILE *f = vfs.fopen(updateFile.c_str(), "r");
                if (f != nullptr) {
                    updateFileDetails << "Size: ";
                    updateFileDetails << std::to_string(vfs.filelength(f) / 1024);
                    updateFileDetails << "Kb (";
                    updateFileDetails << msg.updateStats.versioInformation[purefs::json::misc][purefs::json::builddate]
                                         .string_value();
                    updateFileDetails << ")";
                    vfs.fclose(f);
                }

                currentVersionInfo->setText(currentVersion.str());
                updateVersionInfo->setText(updateVersion.str());
                updateDetails->setText(updateFileDetails.str());
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
                bottomBar->setActive(BottomBar::Side::CENTER, false);
                bottomBar->setActive(BottomBar::Side::RIGHT, false);

                selectionLabels[0]->setVisible(false);
                selectionLabels[1]->setVisible(false);

                percentLabel->setVisible(true);
                percentLabel->setText("Update start");
                auto msgToSend = std::make_shared<sdesktop::UpdateOsMessage>(updateFile.c_str(), 0);
                sys::Bus::SendUnicast (msgToSend, service::name::service_desktop, application);

                return true;
            }
        }

        return false;
    }

    bool UpdateWindow::handleSwitchData(SwitchData *data)
    {
        gui::UpdateSwitchData *item = dynamic_cast<gui::UpdateSwitchData *>(data);
        if (item != nullptr) {
            std::stringstream ssi;
            std::stringstream sizeStream;

            updateos::UpdateState status =
                static_cast<updateos::UpdateState>(item->getUpdateOsMessage().updateStats.status);

            if (status == updateos::UpdateState::ExtractingFiles) {
                progressPercent = static_cast<int>(((float)item->getUpdateOsMessage().updateStats.currentExtractedBytes / (float)item->getUpdateOsMessage().updateStats.totalBytes) * 100.0);
                ssi << "Unpacking: ";
                ssi << std::to_string(progressPercent);
                ssi << " %";
                percentLabel->setText(ssi.str());
                infoLabel->setText(item->getUpdateOsMessage().updateStats.messageText);
            } else if (item->getUpdateOsMessage().updateStats.messageText != "") {
                percentLabel->setText(item->getUpdateOsMessage().updateStats.messageText);
            }

            sizeStream << "size: ";
            sizeStream << std::to_string(item->getUpdateOsMessage().updateStats.fileExtractedSize);
            sizeStream << " bytes";
            detailLabel->setText(sizeStream.str());
            this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }

        return true;
    }
} /* namespace gui */
