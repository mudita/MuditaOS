#include "SearchResultsModel.hpp"
#include "../widgets/SearchResultsItem.hpp"

namespace gui::model
{

    SearchResultsModel::SearchResultsModel(app::Application *app) : DatabaseModel(app)
    {}

    gui::ListItem *SearchResultsModel::getItem(int index)
    {
        auto ret = new gui::SearchResultsItem();
        LOG_INFO("LOL: %d: %s", index, ret->area().str().c_str());
        ret->setTitle("Title: " + std::to_string(index));
        ret->setDate("00:00:0000");
        ret->setPreview("this is preview text");
        ret->setID(index);
        return ret;
    }

}; // namespace gui::model
