// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSoundsModel.hpp"
#include <apps-common/audio/AbstractSoundsPlayer.hpp>

#include <apps-common/AudioOperations.hpp>
#include <Audio/decoder/Decoder.hpp>
#include <InternalModel.hpp>
#include <Application.hpp>

/// Simple SoundsModel
class SoundsModel : public app::InternalModel<gui::ListItem *>, public AbstractSoundsModel
{
  public:
    explicit SoundsModel(std::shared_ptr<AbstractSoundsPlayer> soundsPlayer);

    /// Creates data for model
    /// @param app pointer to current application
    /// @param model audio settings model
    void createData(app::ApplicationCommon *app, audio_settings::AbstractAudioSettingsModel *model) override;

    void clearData() override;

    [[nodiscard]] unsigned int requestRecordsCount() override;

    [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;

    gui::ListItem *getItem(gui::Order order) override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

  protected:
    /// Returns the path where sounds for specified audio settings model are stored
    /// @param model audio settings model
    /// @return path where sounds are stored
    [[nodiscard]] std::filesystem::path getSoundPath(audio_settings::AbstractAudioSettingsModel *model);

    /// Apply the items to internal model
    /// @param sounds collection of sound paths
    /// @param app pointer to current application
    /// @param model audio settings model
    void applyItems(const std::vector<std::filesystem::path> &sounds,
                    app::ApplicationCommon *app,
                    audio_settings::AbstractAudioSettingsModel *model);

    std::shared_ptr<AbstractSoundsPlayer> soundsPlayer;
};
