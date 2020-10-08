#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    class MessagesWindow : public OptionWindow
    {
      public:
        MessagesWindow(app::Application *app);
        ~MessagesWindow() override = default;
        void onBeforeShow(ShowMode m, SwitchData *d) override;
        void rebuildOptList();

      private:
        std::list<Option> messagesOptList();
        bool showUnreadMessagesFirst = false;
    };
} // namespace gui
