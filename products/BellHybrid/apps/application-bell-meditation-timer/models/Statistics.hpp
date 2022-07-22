// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/AsyncTask.hpp>

#include <chrono>
#include <optional>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation::models
{
    struct Summary
    {
        std::chrono::minutes sum;
        std::chrono::minutes avg;
        std::size_t count;
    };

    class Statistics : public AsyncCallbackReceiver
    {
      public:
        explicit Statistics(app::ApplicationCommon *app);
        void addEntry(time_t utcTimestamp, std::chrono::minutes duration);
        std::optional<Summary> getSummary(std::uint32_t days);

      private:
        app::ApplicationCommon *app{nullptr};
    };

} // namespace app::meditation::models
