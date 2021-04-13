// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FotaWindow.hpp"

#include "Fota.hpp"

#include <i18n/i18n.hpp>

namespace gui
{

    FotaWindow::FotaWindow(app::Application *app) : AppWindow(app, window::name::fota_window)
    {
        buildInterface();
        fotaUpdater = std::make_unique<Fota>(this);
        currentFirmwareLabel->setText(fotaUpdater->getCurrentFirmwareVersion());
    }

    FotaWindow::~FotaWindow()
    {
        destroyInterface();
    }

    void FotaWindow::onBeforeShow(ShowMode /*mode*/, SwitchData * /*data*/)
    {}

    void FotaWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void FotaWindow::buildInterface()
    {
        LOG_INFO("Build Fota Window");
        AppWindow::buildInterface();
        setTitle("Modem Firmware update (FOTA)");

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("Go"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate("common_back"));

        mainBox = new gui::VBox(this, 0, title->offset_h(), style::window_width, style::window_height);
        mainBox->setPenWidth(style::window::default_border_no_focus_w);

        addBoxLabel(mainBox, "FOTA Status:");
        statusLabel = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        statusLabel->setFocus(true);

        addBoxLabel(mainBox, "Current Firmware:");
        currentFirmwareLabel = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);

        setFocusItem(statusLabel);

        newFirmwareLabelText = addBoxLabel(mainBox, "New Firmware:");
        newFirmwareLabel     = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        newFirmwareLabel->setText("");

        const unsigned char progressLow(0);
        const unsigned char progressMax(100);
        downloadProgress = new ProgressBar(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        downloadProgress->setMaximum(progressMax);
        downloadProgress->setValue(progressLow);

        statusLabel->activatedCallback = [&](Item &) -> bool {
            if (bottomBar->isActive(gui::BottomBar::Side::CENTER)) {
                fotaUpdater->next();
            }
            return true;
        };
        newFirmwareLabelText->setVisible(false);
        newFirmwareLabel->setVisible(false);
        downloadProgress->setVisible(false);
        getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void FotaWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void FotaWindow::invalidate() noexcept
    {
        currentFirmwareLabel = nullptr;
        statusLabel          = nullptr;
        newFirmwareLabelText = nullptr;
        newFirmwareLabel     = nullptr;
        downloadProgress     = nullptr;
        mainBox              = nullptr;
    }

    BottomBar *FotaWindow::getBottomBar()
    {
        return bottomBar;
    }

    gui::Label *FotaWindow::addBoxLabel(BoxLayout *layout, const std::string &text)
    {
        auto el = new gui::Label(layout, 0, 0, style::window_width, style::window::label::default_h);
        style::window::decorateOption(el);
        el->setText(text);
        return el;
    }

} // namespace gui
