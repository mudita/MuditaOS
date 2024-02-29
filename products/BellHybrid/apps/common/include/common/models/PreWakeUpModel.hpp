// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    class AbstractPreWakeUpModel
    {
      public:
        virtual ~AbstractPreWakeUpModel() noexcept = default;

        virtual bool isActive() const       = 0;
        virtual void setActive(bool active) = 0;
    };

    class PreWakeUpModel : public AbstractPreWakeUpModel
    {
      public:
        bool isActive() const override;
        void setActive(bool active) override;

      private:
        bool activity;
    };
} // namespace app
