// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>
#include <application-desktop/data/LockPhoneData.hpp>
#include <source/version.hpp>

// module-utils
#include <i18n/i18n.hpp>

#include <application-desktop/ApplicationDesktop.hpp>

// services
#include <service-appmgr/model/ApplicationManager.hpp>

#include <Style.hpp>
#include <DialogMetadataMessage.hpp>

#include "UpdateProgress.hpp"

namespace style
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 132;
    } // namespace image

    namespace text
    {
        constexpr uint32_t x = 120;
        constexpr uint32_t y = 250;
        constexpr uint32_t w = 250;
        constexpr uint32_t h = 100;
    } // namespace text

    namespace percentlabel
    {
        constexpr uint32_t x = 0;
        constexpr uint32_t y = 450;
        constexpr uint32_t w = 500;
        constexpr uint32_t h = 100;
    } // namespace percentlabel

    namespace progressbar
    {
        constexpr uint32_t x = 100;
        constexpr uint32_t y = 400;
    } // namespace progressbar

} // namespace style
namespace gui
{

    UpdateProgressWindow::UpdateProgressWindow(app::Application *app)
        : AppWindow(app, app::window::name::desktop_update_progress)
    {
        buildInterface();
    }

    void UpdateProgressWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void UpdateProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_desktop_update_muditaos"));

        icon = new Image(this, style::image::x, style::image::y, "circle_update");

        text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
        text->setTextType(TextType::MultiLine);
        text->setEditMode(EditMode::Browse);
        text->setEdges(RectangleEdge::None);
        text->setFont(style::window::font::biglight);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        percentLabel = new gui::Label(
            this, style::percentlabel::x, style::percentlabel::y, style::percentlabel::w, style::percentlabel::h);
        percentLabel->setFilled(false);
        percentLabel->setBorderColor(gui::ColorNoColor);
        percentLabel->setFont(style::window::font::biglight);
        percentLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        percentLabel->setVisible(true);

        updateProgress = new ProgressBar(this,
                                         style::progressbar::x,
                                         style::progressbar::y,
                                         style::window_width - 2 * style::progressbar::x,
                                         style::window::label::default_h);
        updateProgress->setMaximum(100);
        updateProgress->setValue(0);
        updateProgress->setVisible(true);
    }

    void UpdateProgressWindow::destroyInterface()
    {
        erase();
    }

    void UpdateProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
        }
        else {
            auto *item = dynamic_cast<gui::UpdateSwitchData *>(data);
            if (item != nullptr) {
                auto msg   = item->getUpdateOsMessage();
                updateFile = msg.updateStats.updateFile;
                auto updateVersion =
                    msg.updateStats.versionInformation[boot::json::os_version][boot::json::version_string]
                        .string_value();
                text->setRichText(utils::localize.get("app_desktop_update_preparing") + " " + updateVersion);
            }
        }
        state = State::Return;
    }

    bool UpdateProgressWindow::handleSwitchData(SwitchData *data)
    {
        auto *item = dynamic_cast<gui::UpdateSwitchData *>(data);
        if (item != nullptr) {
            auto status = static_cast<updateos::UpdateState>(item->getUpdateOsMessage().updateStats.status);

            switch (status) {
            case updateos::UpdateState::Initial:
                text->setRichText(text->getText());
                progressPercent = 10;
                break;
            case updateos::UpdateState::UpdateFileSet:
                text->setRichText(text->getText());
                progressPercent = 20;
                break;
            case updateos::UpdateState::CreatingDirectories:
                text->setRichText(text->getText());
                progressPercent = 30;
                break;
            case updateos::UpdateState::ExtractingFiles:
                text->setRichText(utils::localize.get("app_desktop_update_in_progress"));
                progressPercent = 40;
                break;
            case updateos::UpdateState::ChecksumVerification:
                text->setRichText(text->getText());
                progressPercent = 70;
                break;
            case updateos::UpdateState::VersionVerificiation:
                text->setRichText(text->getText());
                progressPercent = 90;
                break;
            case updateos::UpdateState::UpdatingBootloader:
                text->setRichText(text->getText());
                progressPercent = 99;
                break;
            case updateos::UpdateState::ReadyForReset:
                text->setRichText(utils::localize.get("app_desktop_update_ready_for_reset"));
                progressPercent = 100;
                break;
            default:
                if (item->getUpdateOsMessage().updateStats.messageText != "") {
                    percentLabel->setText(item->getUpdateOsMessage().updateStats.messageText);
                }
            }

            percentLabel->setVisible(true);
            percentLabel->setText(std::to_string(progressPercent) + " %");
            updateProgress->setVisible(true);
            updateProgress->setFocus(true);
            updateProgress->setValue(progressPercent);
            text->setVisible(true);
        }
        return true;
    }
} /* namespace gui */
