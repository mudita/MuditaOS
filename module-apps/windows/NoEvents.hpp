#pragma once

#include "AppWindow.hpp"
#include <gui/widgets/Icon.hpp>
#include <Text.hpp>
#include <functional>
enum class EquivalentWindow
{
    DayEventsWindow,
    AllEventsWindow,
    EmptyWindow
};

/// Provides information witch window is equivalent to empty window (DayEvents window and AllEvents window may be empty)
enum class EquivalentWindow
{
    DayEventsWindow,
    AllEventsWindow,
    EmptyWindow
};

namespace gui
{
    /// @brief base NoEvents class
    ///
    /// Contain icon, text, Back action and switch to new/edit event window action
    class NoEvents : public AppWindow
    {
      protected:
        Image *leftArrowImage = nullptr;
        Image *newEventImage  = nullptr;
        Icon *emptyListIcon   = nullptr;

      public:
        struct Meta
        {
            std::string title;
            UTF8 text;
            UTF8 icon;
            std::function<bool()> action = []() -> bool { return false; };

            Meta() = default;
        } meta;

        NoEvents(app::Application *app, const std::string &name, const Meta &meta);
        ~NoEvents() override = default;

        virtual void update(const Meta &meta);
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

}; // namespace gui
