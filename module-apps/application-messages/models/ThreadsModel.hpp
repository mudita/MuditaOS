#pragma once

#include "BaseThreadsRecordModel.hpp"

class ThreadsModel : public BaseThreadsRecordModel
{
  public:
    ThreadsModel(app::Application *app);

    // virtual method for db::QueryListener
    auto handleQueryResponse(db::QueryResult *) -> bool override;

    void requestRecords(uint32_t offset, uint32_t limit) override;
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
};
