#pragma once

#include "../Application.hpp"
#include "AppWindow.hpp"
#include "PageLayout.hpp"
#include <functional>

#include <log/log.hpp>

namespace gui
{
    enum class Arrow : bool
    {
        Disabled,
        Enabled
    };

    struct Option
    {
        const UTF8 text = "";
        std::function<bool(Item &)> activatedCallback = nullptr;
        Arrow arrow = Arrow::Enabled;
        Option(const UTF8 &text, std::function<bool(Item &)> cb, Arrow arrow) : text(text), activatedCallback(cb), arrow(arrow)
        {
            LOG_INFO("text: %s", text.c_str());
        }
    };

    /// creates new `option` label on heap with text description and on activated callback connected
    Item *newOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow = Arrow::Disabled);
    Item *newOptionLabel(const Option &option);

    class OptionWindow : public AppWindow
    {

      protected:
        PageLayout *body = nullptr;

      public:
        OptionWindow(app::Application *app, const std::string &name);
        virtual ~OptionWindow();

        void addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow);
        /// add options as {text, on click callback }
        void addOptions(std::list<Option> options);
        /// add options as list of items on heap
        /// [!] ovnership on item will be moved to OptionWindow (so it will clean created kids on heap)
        void addOptions(std::list<Item *> items);
        void clearOptions();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

    /// default name will set name to Options from i18
    OptionWindow *newOptionWindow(app::Application *app, std::string name = "", std::list<Option> options = {});

}; // namespace gui
