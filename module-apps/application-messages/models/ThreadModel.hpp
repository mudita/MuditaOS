#pragma once

#include "BaseThreadRecordModel.hpp"

class ThreadModel : public BaseThreadRecordModel
{
  public:
    ThreadModel(app::Application *app);

    unsigned int getMinimalItemHeight() override;
    gui::ListItem *getItem(gui::Order order) override;
};
