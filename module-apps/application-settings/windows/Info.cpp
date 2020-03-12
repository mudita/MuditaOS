#include "Info.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <Style.hpp>
#include <memory>

#include <../source/version.hpp>

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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle("Info");

        box = new gui::VBox(this, 0, title->offset_h(), style::window_width, 5 * style::window::label::default_h);
        box->setPenWidth(style::window::default_border_no_focus_w);

        add_box_label(box, "GIT revision: " + std::string(GIT_REV));
        add_box_label(box, "GIT tag:      " + std::string(GIT_TAG));
        add_box_label(box, "GIT branch:   " + std::string(GIT_BRANCH));
        box->resizeItems();
        // this->focusItem->setFocus(true);
    }

    void Info::destroyInterface()
    {
        this->focusItem = nullptr;
        AppWindow::destroyInterface();
    }

    Info::~Info()
    {
        destroyInterface();
    }

    void Info::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

} // namespace gui
