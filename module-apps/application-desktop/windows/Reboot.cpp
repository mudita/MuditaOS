#include "Reboot.hpp"
#include "../ApplicationDesktop.hpp"
#include <Style.hpp>
#include <i18/i18.hpp>

namespace gui
{

    RebootWindow::RebootWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_reboot)
    {
        buildInterface();
    }

    void RebootWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void RebootWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto text_y_offset = 270;
        auto text_height   = 300;

        text = new Text(
            this,
            style::window::default_left_margin,
            text_y_offset,
            style::window_width - style::window::default_left_margin * 2,
            text_height,
            "Phone have to reboot, press any key to confirm and remove battery for 10 sec to perform full reboot");
        text->setFilled(false);
        text->setBorderColor(gui::ColorFullBlack);
        text->setFont(style::header::font::title);
        text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        text->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
    }

    void RebootWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    void RebootWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_FATAL("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    bool RebootWindow::onInput(const InputEvent &inputEvent)
    {
        text->setText("!!! Shutdown !!!");
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        /// needed to actually have time to show stuff on screen before system close
        ulTaskNotifyTake(pdTRUE, 1000);
        // shutdown
        sys::SystemManager::CloseSystem(application);
        return true;
    }

} /* namespace gui */
