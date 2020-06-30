#include "Info.hpp"

#include <application-settings/ApplicationSettings.hpp>

#include <service-appmgr/ApplicationManager.hpp>
#include <module-services/service-cellular/api/CellularServiceAPI.hpp>

#include <source/version.hpp>

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Margins.hpp>
#include <gui/widgets/Style.hpp>

#include <i18/i18.hpp>

#include <memory>

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

    void add_box_label(BoxLayout *layout, const std::string &text)
    {
        auto el = new gui::Label(layout, 0, 0, style::window_width, style::window::label::default_h);
        style::window::decorateOption(el);
        el->setText(text);
    }

    void Info::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle("Info");

        box = new gui::VBox(this, 0, title->offset_h(), style::window_width, 7 * style::window::label::default_h);
        box->setPenWidth(style::window::default_border_no_focus_w);

        addAllignedLabelWithValue(box, "GIT revision:", std::string(GIT_REV));
        addAllignedLabelWithValue(box, "GIT tag:", std::string(GIT_TAG));
        addAllignedLabelWithValue(box, "GIT branch:", std::string(GIT_BRANCH));
        addAllignedLabelWithValue(box, "Version:", std::string(VERSION));
        add_box_label(box, "Modem Firmware:");
        std::string firmwareVersion;
        CellularServiceAPI::GetFirmwareVersion(getApplication(), firmwareVersion);
        add_box_label(box, firmwareVersion);
        LOG_DEBUG("Modem Firmware: %s", firmwareVersion.c_str());
        /// TODO: make sure entire string is displayed.
        box->resizeItems();
    }

    void Info::destroyInterface()
    {
        erase();
    }

    void Info::addAllignedLabelWithValue(BoxLayout *layout, const string &labelText, const string &valueText)
    {
        auto lineBox = new gui::HBox(layout, 0, 0, style::window_width, style::window::label::default_h);

        lineBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        auto label = new gui::Label(nullptr, 0, 0, style::window_width * 0.35, style::window::label::default_h);
        style::window::decorateOption(label);
        label->setText(labelText);

        auto value = new gui::Label(nullptr, 0, 0, style::window_width * 0.64, style::window::label::default_h);
        style::window::decorateOption(value);
        value->setText(valueText);

        lineBox->addWidget(label);
        lineBox->addWidget(value);

        LOG_DEBUG("%s:%s", labelText.c_str(), valueText.c_str());
    }

    void Info::onBeforeShow(ShowMode /*mode*/, SwitchData * /*data*/)
    {}

} // namespace gui
