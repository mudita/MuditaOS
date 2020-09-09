#pragma once

#include "application-messages/models/ThreadsModel.hpp"

#include <AppWindow.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListView.hpp>
#include <Text.hpp>
#include <gui/widgets/Icon.hpp>

#include <functional>
#include <string>

namespace gui
{
    class MessagesMainWindow : public AppWindow
    {
      protected:
        Image *leftArrowImage  = nullptr;
        Image *rightArrowImage = nullptr;
        Image *newMessageImage = nullptr;
        Image *searchImage     = nullptr;

        std::shared_ptr<ThreadsModel> threadsModel = nullptr;
        gui::ListView *list                      = nullptr;

        Icon *emptyListIcon = nullptr;

      public:
        MessagesMainWindow(app::Application *app);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
