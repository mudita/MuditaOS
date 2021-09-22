// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
}

namespace app::bgSounds
{
    class BGSoundsVolumeModel;
    class BGSoundsVolumeContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<BGSoundsVolumeContract::View>
        {
          public:
            virtual auto getVolumeProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual void loadVolumeData()                                              = 0;
            virtual void onVolumeChanged()                                             = 0;
        };
    };

    class BGSoundsVolumePresenter : public BGSoundsVolumeContract::Presenter
    {
        std::shared_ptr<BGSoundsVolumeModel> model;
        auto getVolumeProvider() -> std::shared_ptr<gui::ListItemProvider> override;
        void loadVolumeData() override;
        void onVolumeChanged() override;

      public:
        BGSoundsVolumePresenter();
    };
} // namespace app::bgSounds
