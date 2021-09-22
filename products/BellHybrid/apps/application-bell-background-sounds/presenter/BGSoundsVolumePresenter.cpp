// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumePresenter.hpp"
#include "models/BGSoundsVolumeModel.hpp"

namespace app::bgSounds
{
    BGSoundsVolumePresenter::BGSoundsVolumePresenter() : model{std::make_shared<BGSoundsVolumeModel>()}
    {}
    auto BGSoundsVolumePresenter::getVolumeProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }
    void BGSoundsVolumePresenter::loadVolumeData()
    {
        model->createData();
    }
    void BGSoundsVolumePresenter::onVolumeChanged()
    {}
} // namespace app::bgSounds
