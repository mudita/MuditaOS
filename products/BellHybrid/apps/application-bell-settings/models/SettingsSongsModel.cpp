// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsSongsModel.hpp"
#include <common/widgets/LabelOption.hpp>

namespace app::bell_settings
{
    SettingsSongsModel::SettingsSongsModel(ApplicationCommon *application,
                                           std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                                           const LabelsWithPaths &pathPrefixes)
        : SongsModel{application, std::move(soundsRepository), pathPrefixes}
    {}

    void SettingsSongsModel::createData(OnActivateCallback activateCallback, OnScrollCallback scrollCallback)
    {
        this->scrollCallback = scrollCallback;
        SongsModel::createData(activateCallback);
    }

    gui::ListItem *SettingsSongsModel::getItem(gui::Order order)
    {
        const auto sound = getRecord(order);
        if (!sound) {
            return nullptr;
        }
        auto item = gui::option::LabelOption{getLabelFromPath(sound->fileInfo.path),
                                             sound->tags.title,
                                             [=]([[maybe_unused]] gui::Item &item) {
                                                 if (activateCallback) {
                                                     activateCallback(*sound);
                                                 }
                                                 return true;
                                             },
                                             [=]([[maybe_unused]] gui::Item &item) {
                                                 if (item.focus && scrollCallback) {
                                                     scrollCallback(*sound);
                                                 }
                                                 return true;
                                             },
                                             nullptr};

        return item.build();
    }

} // namespace app::bell_settings
