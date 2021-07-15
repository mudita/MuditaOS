﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItemProvider.hpp>
#include <application-settings/models/AudioSettingsModel.hpp>

namespace app
{
    class Application;
};

/// @brief Abstract SoundsModel class
class AbstractSoundsModel : public gui::ListItemProvider
{
  public:
    virtual ~AbstractSoundsModel() = default;

    /// Creates data for model
    /// @param app pointer to current application
    /// @param model audio settings model
    virtual void createData(app::Application *app, audio_settings::AbstractAudioSettingsModel *model) = 0;

    /// Clears all data for model
    virtual void clearData() = 0;
};
