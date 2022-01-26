// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>
#include <service-db/Settings.hpp>
#include <string>

namespace app
{
    class ApplicationCommon;
}

namespace app::bell_settings
{

    class AbstractLayoutModel
    {
      public:
        virtual ~AbstractLayoutModel()                        = default;
        virtual std::string getValue() const                  = 0;
        virtual void setValue(const std::string &value) const = 0;
    };

    class LayoutModel : public AbstractLayoutModel
    {
      public:
        explicit LayoutModel(ApplicationCommon *app);
        std::string getValue() const override;
        void setValue(const std::string &value) const override;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::bell_settings