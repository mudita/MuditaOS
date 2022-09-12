// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gui
{
    class ListItemProvider;
}
namespace app
{
    class ApplicationCommon;
}
namespace app::music
{
    class AbstractSongsRepository;
}
namespace app::bgSounds
{
    class SoundsProvider;

    class BGSoundsMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<BGSoundsMainWindowContract::View>
        {
          public:
            virtual std::shared_ptr<gui::ListItemProvider> getProvider() = 0;
        };
    };

    class BGSoundsMainWindowPresenter : public BGSoundsMainWindowContract::Presenter
    {
      public:
        BGSoundsMainWindowPresenter(app::ApplicationCommon *app,
                                    std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository);

      private:
        std::shared_ptr<SoundsProvider> soundsProvider;

        std::shared_ptr<gui::ListItemProvider> getProvider() override;
    };

} // namespace app::bgSounds
