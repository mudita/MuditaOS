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
        buildInterface();
    }

    void OptionWindow::rebuild()
    {}

    Item *newOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow)
    {
        // TODO fix elements positioning with styles ready, right now moved from Settings main window as it is
        gui::Label *label =
            new gui::Label(nullptr, 20, 0, style::window_width - 2 * 20, style::window::label::big_h, text);
        style::window::decorateOption(label);
        label->activatedCallback = activatedCallback;
        if (arrow == Arrow::Enabled) {
            new gui::Image(label, 425 - 17, 24, 0, 0, "right_label_arrow");
        }
        return label;
    }

    Item *newOptionLabel(const Option &option)
    {
        return newOptionLabel(option.text, option.activatedCallback, option.arrow);
    }

    void OptionWindow::addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow)
    {
        body->addWidget(newOptionLabel(text, activatedCallback, arrow));
    }

    void OptionWindow::addOptions(std::list<Option> options)
    {
        for (auto &el : options) {
            LOG_INFO("adding option: %s", el.text.c_str());
            addOptionLabel(el.text, el.activatedCallback, el.arrow);
        }
        body->switchPage(0);
    }

    void OptionWindow::addOptions(std::list<Item *> items)
    {
        for (auto &el : items) {
            body->addWidget(el);
        }
        body->switchPage(0);
    }

    void OptionWindow::clearOptions()
    {
        body->erase();
    }

    void OptionWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        setTitle(name);

        // magical offset on designs
        int32_t offset_h = 8;

        body = new gui::PageLayout(this,
                                   {0,
                                    title->offset_h() + offset_h,
                                    this->getWidth(),
                                    this->getHeight() - offset_h - this->title->offset_h() - bottomBar->getHeight()});

        setFocusItem(body);
    }
    void OptionWindow::destroyInterface()
    {
        erase();
    }

    OptionWindow::~OptionWindow()
    {
        destroyInterface();
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    OptionWindow *newOptionWindow(app::Application *app, std::string name, std::list<Option> options)
    {
        if (name == "") {
            LOG_DEBUG("no name for window - take default");
            name = utils::localize.get("app_phonebook_options_title");
        }
        auto window = new OptionWindow(app, name);
        window->addOptions(options);
        return window;
    }

} /* namespace gui */
