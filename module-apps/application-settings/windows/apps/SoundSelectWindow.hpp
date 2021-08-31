// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/apps/AbstractSoundsModel.hpp>
#include <application-settings/models/apps/AbstractSoundsPlayer.hpp>
#include <AppWindow.hpp>

namespace gui
{
    class ListView;

    ///  @brief Sound selection window
    class SoundSelectWindow : public AppWindow
    {
      public:
        ///  Constructor
        ///  @param app pointer to application
        ///  @param name window name
        explicit SoundSelectWindow(app::Application *app, std::string name, std::shared_ptr<AbstractSoundsModel> model);

        ///  Called before the window is shown
        ///  @param mode show mode
        ///  @param data user switch data
        void onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        std::shared_ptr<AbstractSoundsModel> mSoundsModel = nullptr;
        ListView *mSoundsList                             = nullptr;
    };
} // namespace gui
