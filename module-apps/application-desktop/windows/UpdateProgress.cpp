// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <source/version.hpp>
#include <boot/bootconfig.hpp>

// module-utils
#include <i18n/i18n.hpp>

#include <application-desktop/ApplicationDesktop.hpp>
#include <application-desktop/data/DesktopStyle.hpp>
#include <application-desktop/data/DesktopData.hpp>

// services
#include <service-appmgr/model/ApplicationManager.hpp>

#include <Style.hpp>
#include <DialogMetadataMessage.hpp>
#include "UpdateProgress.hpp"

namespace gui
{
    UpdateProgressWindow::UpdateProgressWindow(app::Application *app)
        : AppWindow(app, app::window::name::desktop_update_progress)
    {
        buildInterface();
        preventsAutoLock = true;
    }

    void UpdateProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
        }
        else {
            auto *item = dynamic_cast<gui::UpdateSwitchData *>(data);
            if (item != nullptr) {
                const auto &msg = item->getUpdateOsMessage();
                updateFile = msg.updateStats.updateFile;
                auto updateVersion =
                    msg.updateStats.versionInformation[boot::json::os_version][boot::json::version_string]
                        .string_value();
                if (text->getText().empty()) {
                    text->setText(utils::translate("app_desktop_update_preparing") + " " + updateVersion);
                }
                else {
                    text->setText(textInfo);
                }
            }
            percentLabel->setText(std::to_string(progressPercent) + " %");
            updateProgress->setPercentageValue(progressPercent);
        }
        setVisibleState();
    }

    void UpdateProgressWindow::setVisibleState()
    {
        percentLabel->setVisible(true);
        updateProgress->setVisible(true);
        updateProgress->setVisible(true);
        text->setVisible(true);
    }

    void UpdateProgressWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    status_bar::Configuration UpdateProgressWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::Time);
        appConfiguration.disable(status_bar::Indicator::Lock);
        appConfiguration.disable(status_bar::Indicator::Battery);
        appConfiguration.disable(status_bar::Indicator::Signal);
        appConfiguration.disable(status_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void UpdateProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_desktop_update_muditaos"));

        icon = new Image(this, style::desktop::image::x, style::desktop::image::y, "circle_update");

        text = new Text(this,
                        style::desktop::textupdate::x,
                        style::desktop::textupdate::y,
                        style::desktop::textupdate::w,
                        style::desktop::textupdate::h);
        text->setTextType(TextType::MultiLine);
        text->setEditMode(EditMode::Browse);
        text->setEdges(RectangleEdge::None);
        text->setFont(style::window::font::biglight);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        percentLabel = new gui::Label(this,
                                      style::desktop::percentlabel::x,
                                      style::desktop::percentlabel::y,
                                      style::desktop::percentlabel::w,
                                      style::desktop::percentlabel::h);
        percentLabel->setFilled(false);
        percentLabel->setBorderColor(gui::ColorNoColor);
        percentLabel->setFont(style::window::font::biglight);
        percentLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        updateProgress = new HBarGraph(this,
                                       style::window::progressBar::x,
                                       style::window::progressBar::y,
                                       style::window::progressBar::range,
                                       BarGraphStyle::Light);
        updateProgress->setSize(style::window::progressBar::w, style::window::progressBar::h);
        updateProgress->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    }

    void UpdateProgressWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void UpdateProgressWindow::invalidate() noexcept
    {
        percentLabel   = nullptr;
        updateProgress = nullptr;
        text           = nullptr;
        icon           = nullptr;
    }

    bool UpdateProgressWindow::handleSwitchData(SwitchData *data)
    {
        auto *item = dynamic_cast<gui::UpdateSwitchData *>(data);
        if (item != nullptr) {
            auto status = static_cast<updateos::UpdateState>(item->getUpdateOsMessage().updateStats.status);
            switch (status) {
            case updateos::UpdateState::Initial:
                textInfo        = text->getText();
                progressPercent = 10;
                break;
            case updateos::UpdateState::UpdateFileSet:
                textInfo        = text->getText();
                progressPercent = 20;
                break;
            case updateos::UpdateState::CreatingDirectories:
                textInfo        = text->getText();
                progressPercent = 30;
                break;
            case updateos::UpdateState::ExtractingFiles:
                textInfo        = utils::translate("app_desktop_update_in_progress");
                progressPercent = 40;
                break;
            case updateos::UpdateState::ChecksumVerification:
                textInfo        = text->getText();
                progressPercent = 70;
                break;
            case updateos::UpdateState::VersionVerificiation:
                textInfo        = text->getText();
                progressPercent = 80;
                break;
            case updateos::UpdateState::UpdatingBootloader:
                textInfo        = utils::translate("app_desktop_update_ready_for_reset");
                progressPercent = 99;
                break;
            case updateos::UpdateState::ReadyForReset:
                textInfo        = text->getText();
                progressPercent = 100;
                break;
            }
        }
        return true;
    }
} /* namespace gui */
