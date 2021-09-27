// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

#include "MeditationItem.hpp"
#include "MeditationBaseModel.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class MeditationBaseContract
    {
      public:
        class View
        {
          public:
            virtual ~View()              = default;
            virtual void updateDisplay() = 0;
        };

        class Presenter : public BasePresenter<MeditationBaseContract::View>
        {
          public:
            virtual ~Presenter() noexcept          = default;
            virtual void set(MeditationItem &item) = 0;
            virtual void get(MeditationItem &item) = 0;
        };
    };

    class MeditationBasePresenter : public MeditationBaseContract::Presenter
    {
      public:
        explicit MeditationBasePresenter(app::ApplicationCommon *app);
        void set(MeditationItem &item) override;
        void get(MeditationItem &item) override;

      protected:
        app::ApplicationCommon *app{};
        std::shared_ptr<MeditationBaseModel> model{nullptr};
    };
} // namespace app::meditation
