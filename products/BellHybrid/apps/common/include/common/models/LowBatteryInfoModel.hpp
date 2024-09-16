// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace app
{
    class AbstractLowBatteryInfoModel
    {
      public:
        virtual ~AbstractLowBatteryInfoModel() noexcept = default;

        [[nodiscard]] virtual bool isInfoHandled() const = 0;
        virtual void handleInfo()                        = 0;
    };

    class LowBatteryInfoModel : public AbstractLowBatteryInfoModel
    {
      public:
        [[nodiscard]] bool isInfoHandled() const override;
        void handleInfo() override;

      private:
        bool lowBatteryInfoHandled{false};
    };
} // namespace app
