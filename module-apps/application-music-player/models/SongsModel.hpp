#pragma once

#include "module-apps/application-music-player/data/MusicPlayerStyle.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>

class SongsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    SongsModel(app::Application *app);

    void clearData();
    void createData(std::vector<audio::Tags> songsList, std::function<bool(const std::string &fileName)>);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
