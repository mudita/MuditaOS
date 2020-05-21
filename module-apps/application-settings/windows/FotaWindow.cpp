#include "FotaWindow.hpp"

#include "Fota.hpp"

#include <service-cellular/api/CellularServiceAPI.hpp>
#include <i18/i18.hpp>

namespace gui
{

    FotaWindow::FotaWindow(app::Application *app) : AppWindow(app, window::fota_window)
    {
        buildInterface();
        fotaWorker = std::make_unique<Fota>(this);
        currentFirmwareLabel->setText(fotaWorker->currentFirmwareVersion());
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

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("Check for updates"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        mainBox = new gui::VBox(this, 0, title->offset_h(), style::window_width, style::window_height
                                /*8 * style::window::label::default_h*/); // style::window_height);
        mainBox->setPenWidth(style::window::default_border_no_focus_w);

        add_box_label(mainBox, "FOTA Status:");
        statusLabel = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        statusLabel->setFocus(true);

        add_box_label(mainBox, "Current Firmware:");
        currentFirmwareLabel = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);

        setFocusItem(statusLabel);

        newFirmwareLabelText = add_box_label(mainBox, "New Firmare:");
        newFirmwareLabelText->setVisible(false);
        newFirmwareLabel = new Label(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        newFirmwareLabel->setText("");
        newFirmwareLabel->setVisible(false);

        downloadProgress = new Progress(mainBox, 0, 0, style::window_width, style::window::label::default_h);
        downloadProgress->setTotalProgress(100);
        downloadProgress->setCurrentPercent(0);
        downloadProgress->setVisible(false);

        statusLabel->activatedCallback = [&](Item &) -> bool {
            fotaWorker->next();
            return true;
        };
    }

    void FotaWindow::destroyInterface()
    {
        this->focusItem = nullptr;
        AppWindow::destroyInterface();
    }

    gui::Label *FotaWindow::add_box_label(BoxLayout *layout, const std::string &text)
    {
        auto el = new gui::Label(layout, 0, 0, style::window_width, style::window::label::default_h);
        style::window::decorateOption(el);
        el->setText(text);
        return el;
    }

} // namespace gui
