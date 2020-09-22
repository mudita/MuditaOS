#pragma once

#include <list>
#include "Item.hpp"
#include "Common.hpp"
#include "SwitchData.hpp"

namespace gui
{

    /// Base window for all UI windows
    ///
    /// It consists of:
    /// 1. TopBar
    /// 2. BottomBar
    /// 3. Body defined per window between TopBar and BottomBar
    ///
    /// All window switches are done based on Window::name and SwitchData
    /// All windows are statically build at start of application, and removed on application end
    /// To rebuild window one have to:
    ///     1. set navigationItem to nullptr (othervise window will crash trying to execute focus callback on reomved
    ///     element)
    ///     2. Remove elements
    ///     3. Add elements anew
    /// or:
    ///     2. handle elements update
    class Window : public Item
    {
      protected:
        RefreshModes refreshMode;
        /// name of window used for windows switching
        std::string name;

      public:
        explicit Window(std::string name);

        /// run every time in Application prior to showing window, except for when we came back to first window with no
        /// switch data to show
        /// @note this is most likely being duplicated by handleSwitchData
        virtual void onBeforeShow(ShowMode mode, SwitchData *data);
        virtual void onClose();
        virtual void getRefreshArea(RefreshModes &mode, uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h);

        /// run prior to onBeforeShow
        /// @note this is most likely duplicate of onBeforeShow
        virtual bool handleSwitchData(SwitchData *data);

        /// This method rebuilds window using updated phone's settings. Internal state must be preserved.
        virtual void rebuild() = 0;
        /// Method to build window's interface
        virtual void buildInterface() = 0;
        /// Method to remove window's interface
        virtual void destroyInterface() = 0;

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        std::list<DrawCommand *> buildDrawList() override;
        std::string getName()
        {
            return name;
        };
    };

} /* namespace gui */
