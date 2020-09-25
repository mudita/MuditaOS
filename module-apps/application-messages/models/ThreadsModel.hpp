#pragma once

#include "BaseThreadsRecordModel.hpp"

class ThreadsModel : public BaseThreadsRecordModel
{
  public:
    explicit ThreadsModel(app::Application *app);

    void requestRecords(uint32_t offset, uint32_t limit) override;
    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
    [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

    auto handleQueryResponse(db::QueryResult *queryResult) -> bool;
};
