#include "Info.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <module-services/service-cellular/api/CellularServiceAPI.hpp>

#include <source/version.hpp>
#include <log/log.hpp>

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Margins.hpp>
#include <gui/widgets/Style.hpp>

#include <i18/i18.hpp>


namespace gui
{

    Info::Info(app::Application *app) : AppWindow(app, gui::window::hw_info)
    {
        buildInterface();
    }

    void Info::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void Info::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle("Info");

        box = new gui::VBox(this, 0, title->offset_h(), style::window_width, style::window_height);
        box->setPenWidth(style::window::default_border_no_focus_w);

        addAlignedLabelWithValue(box, "GIT revision:", std::string(GIT_REV));
        addAlignedLabelWithValue(box, "GIT tag:", std::string(GIT_TAG));
        addAlignedLabelWithValue(box, "GIT branch:", std::string(GIT_BRANCH));
        addAlignedLabelWithValue(box, "Version:", std::string(VERSION));
        addAlignedLabelWithValue(box,
                                 "Bootloader:",
                                 (vfs.getBootConfig().bootloader_verion.empty()
                                      ? utils::localize.get("not available")
                                      : vfs.getBootConfig().bootloader_verion));

        std::string firmwareVersion;
        CellularServiceAPI::GetFirmwareVersion(getApplication(), firmwareVersion);
        addAlignedLabelWithValue(
            box, "Modem Frimware:", (firmwareVersion.empty() ? utils::localize.get("not available") : firmwareVersion));
    }

    void Info::destroyInterface()
    {
        erase();
        invalidate();
    }

    void Info::invalidate() noexcept
    {
        box = nullptr;
    }

    void Info::addAlignedLabelWithValue(BoxLayout *layout, const string &labelText, const string &valueText)
    {
        auto lineBox = new gui::VBox(layout, 0, 0, style::window_width, style::window::label::small_h * 2);
        lineBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        auto label = new gui::Label(lineBox, 0, 0, 0, 0);
        label->setMinimumHeight(style::window::label::small_h);
        label->setMaximumWidth(style::window_width);
        label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setMargins(gui::Margins(style::window::default_left_margin, 0, 0, 0));
        label->setText(labelText);
        label->setFont(style::window::font::smallbold);

        auto value = new gui::Label(lineBox, 0, 0, 0, 0);
        value->setMinimumHeight(style::window::label::small_h);
        value->setMaximumWidth(style::window_width);
        value->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        value->setMargins(gui::Margins(0, 0, style::window::default_right_margin, 0));
        value->setAlignment(gui::Alignment::Horizontal::Right);
        value->setText(valueText);
        value->setFont(style::window::font::small);

        lineBox->resizeItems();
        LOG_DEBUG("%s:%s", labelText.c_str(), valueText.c_str());
    }

} // namespace gui
