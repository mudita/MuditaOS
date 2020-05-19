#pragma once

#include "BaseThreadRecordModel.hpp"

namespace gui::model
{

    class SearchResultsModel : public BaseThreadRecordModel
    {
      public:
        SearchResultsModel(app::Application *app);
        gui::ListItem *getItem(int index) override;
        void requestRecordsCount() override;
    };
}; // namespace gui::model
