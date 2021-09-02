// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/apps/AbstractSoundsModel.hpp>

///  @brief Switch data for SoundSelectWindow which allows
///  to specify its behaviour
class SoundSelectData : public gui::SwitchData
{
  public:
    /// @brief internal struct to carry info for sound selection
    struct Info
    {
        UTF8 windowTitle;
        audio_settings::AbstractAudioSettingsModel *audioModel = nullptr;
    };

    /// Constructor
    /// @param info information to be carried
    explicit SoundSelectData(Info info) : mInfo(std::move(info))
    {}

    /// Returns the struct carrying info
    /// @return information carried by this switch data
    [[nodiscard]] const Info &get() const noexcept
    {
        return mInfo;
    }

  private:
    /// Information carried by this switch data
    Info mInfo;
};
