#pragma once

#include "../widgets/Icon.hpp"
#include "../widgets/ThreadModel.hpp"

#include <AppWindow.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListView.hpp>
#include <Text.hpp>

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

        ThreadModel *threadModel = nullptr;
        gui::ListView *list      = nullptr;

        Icon *emptyListIcon = nullptr;

      public:
        MessagesMainWindow(app::Application *app);
        virtual ~MessagesMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */

