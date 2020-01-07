#include "OptionWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <Style.hpp>
#include <functional>
#include <memory>

namespace gui
{

    OptionWindow::OptionWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
    {
        setSize(style::window_width, style::window_height);
        buildInterface();
    }

    void OptionWindow::rebuild()
    {
    }

    Item *newOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback)
    {
        gui::Label *label = new gui::Label(nullptr, 20, 0, style::window_width - 2 * 20, style::window::label::big_h, text);
        style::window::decorateOption(label);
        label->activatedCallback = activatedCallback;
        new gui::Image(label, 425 - 17, 24, 0, 0, "right_label_arrow");
        return label;
    }

    Item *newOptionLabel(const Option &option)
    {
        return newOptionLabel(option.text, option.activatedCallback);
    }

    void OptionWindow::addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback)
    {
        body->addWidget(newOptionLabel(text, activatedCallback));
    }

    void OptionWindow::addOptions(std::list<Option> options)
    {
        for (auto &el : options)
        {
            LOG_INFO("adding option: %s", el.text.c_str());
            addOptionLabel(el.text, el.activatedCallback);
        }
        body->switchPage(0);
    }

    void OptionWindow::addOptions(std::list<Item *> items)
    {
        for (auto &el : items)
        {
            body->addWidget(el);
        }
        body->switchPage(0);
    }

    void OptionWindow::clearOptions()
    {

        for (auto el : body->children)
        {
            delete el;
        }
        body->children.clear();
    }

    void OptionWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_settings_title_main"));

        body = new gui::PageLayout(this, {0, title->offset_h(), this->getWidth(), this->getHeight() - this->title->offset_h() - bottomBar->getHeight()});

        setFocusItem(body);
    }
    void OptionWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        this->focusItem = nullptr;
        children.clear();
    }

    OptionWindow::~OptionWindow()
    {
        destroyInterface();
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
    }

    OptionWindow *newOptionWindow(app::Application *app, std::string name, std::list<Option> options)
    {
        auto window = new OptionWindow(app, name);
        window->addOptions(options);
        return window;
    }

} /* namespace gui */
