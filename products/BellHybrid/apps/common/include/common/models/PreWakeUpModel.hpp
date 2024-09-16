// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    class AbstractPreWakeUpModel
    {
      public:
        virtual ~AbstractPreWakeUpModel() noexcept = default;

        virtual bool isActive() const       = 0;
        virtual void setActive(bool active) = 0;
        virtual bool turnOffPreWakeUp()     = 0;
    };

    class PreWakeUpModel : public AbstractPreWakeUpModel
    {
      public:
        explicit PreWakeUpModel(ApplicationCommon *app);

        bool isActive() const override;
        void setActive(bool active) override;
        bool turnOffPreWakeUp() override;

      private:
        ApplicationCommon *app{};
        bool activity{false};
    };
} // namespace app
