// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>
#include <common/models/AudioModel.hpp>

namespace app::focus::models
{
    class NotificationVolumeModel : public gui::AbstractSettingsModel<std::uint8_t>
    {
      public:
        explicit NotificationVolumeModel(AbstractAudioModel &audioModel);

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
        void restoreDefault() override;

      private:
        AbstractAudioModel &audioModel;
        std::uint8_t defaultValue;
    };
} // namespace app::focus::models
