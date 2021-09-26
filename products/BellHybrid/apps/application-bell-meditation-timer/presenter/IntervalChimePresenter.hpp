// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

#include "MeditationItem.hpp"
#include "IntervalChimeModel.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class IntervalChimeContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<IntervalChimeContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                        = default;
            virtual void set(MeditationItem &item)                               = 0;
            virtual void get(MeditationItem &item)                               = 0;
            virtual auto getProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;
        };
    };

    class IntervalChimePresenter : public IntervalChimeContract::Presenter
    {
      public:
        explicit IntervalChimePresenter(app::ApplicationCommon *app);

        void set(MeditationItem &item) override;
        void get(MeditationItem &item) override;
        auto getProvider() -> std::shared_ptr<gui::ListItemProvider> override;

      private:
        app::ApplicationCommon *app = nullptr;
        std::shared_ptr<IntervalChimeModel> model;
    };
} // namespace app::meditation
