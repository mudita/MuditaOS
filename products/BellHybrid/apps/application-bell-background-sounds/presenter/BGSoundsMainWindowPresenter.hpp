// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app::bgSounds
{
    class BGSoundsMainWindowContract
    {
      public:
        class View
        {
          public:
            ~View() = default;
        };

        class Presenter : public BasePresenter<BGSoundsMainWindowContract::View>
        {};
    };

    class BGSoundsMainWindowPresenter : public BGSoundsMainWindowContract::Presenter
    {};
} // namespace app::bgSounds
