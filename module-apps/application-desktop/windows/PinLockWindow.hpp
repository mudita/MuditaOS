#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui
{

    class PinLockWindow : public AppWindow
    {
        const uint32_t maxPasswordAttempts = 4;
        enum class State
        {
            EnteringPin,
            WrongPinInfo,
            PhoneBlocked
        };

        gui::Label *titleLabel = nullptr;
        std::vector<gui::Label *> infoLabels;
        std::vector<gui::Label *> pinLabels;
        gui::Image *lockImage      = nullptr;
        gui::Image *infoImage      = nullptr;
        uint32_t remainingAttempts = maxPasswordAttempts;
        // code of the entered character on specified position
        uint32_t charValue[4] = {0};
        // flag defines number of entered characters
        uint32_t charCount = 0;
        // state of the window
        State state = State::EnteringPin;

        std::string lockTimeoutApplilcation = "";

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState(const State &state);

      public:
        PinLockWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
