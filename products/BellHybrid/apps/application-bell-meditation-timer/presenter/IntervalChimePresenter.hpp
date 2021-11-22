// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

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
            virtual ~Presenter() noexcept                          = default;
            virtual std::vector<std::string> getIntervals() const  = 0;
            virtual std::string getCurrentInterval() const         = 0;
            virtual std::string getTimeUnitName(std::string value) = 0;
            virtual void activate(std::string value)               = 0;
        };
    };

    class IntervalChimePresenter : public IntervalChimeContract::Presenter
    {
      public:
        IntervalChimePresenter(app::ApplicationCommon *app, settings::Settings *settings);

        std::vector<std::string> getIntervals() const override;
        std::string getCurrentInterval() const override;
        std::string getTimeUnitName(std::string value) override;
        void activate(std::string value) override;

      private:
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
        std::vector<std::pair<std::chrono::minutes, const std::string>> intervalOptions;

        void initIntervalOptions();
        std::string getIntervalString(std::uint32_t value);
    };
} // namespace app::meditation
