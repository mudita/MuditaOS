// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/application-music-player/data/MusicPlayerStyle.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>
#include <Audio/decoder/Decoder.hpp>

class SongsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    explicit SongsModel(app::Application *app);

    void clearData();
    void createData(const std::vector<audio::Tags> &songsList, std::function<bool(const std::string &fileName)>);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
