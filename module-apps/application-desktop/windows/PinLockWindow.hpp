#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "PinLockBox.hpp"
#include "PinLockBaseWindow.hpp"
namespace gui
{

    class PinLockWindow : public PinLockBaseWindow
    {
        const std::string this_window_name;
        std::string lockTimeoutApplication  = "";
        std::unique_ptr<PinLockBox> LockBox = nullptr;
        PinLock::LockType currentLock       = PinLock::LockType::Unknown;

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState(const PinLock::State state);
        void buildPinLockBox();
        void invalidate() noexcept;

      public:
        PinLockWindow(app::Application *app, const std::string &window_name, PinLock &lock);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
