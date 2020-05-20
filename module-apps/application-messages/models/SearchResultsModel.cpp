#include "SearchResultsModel.hpp"
#include "../widgets/SearchResultsItem.hpp"

namespace gui::model
{

    SearchResultsModel::SearchResultsModel(app::Application *app) : DatabaseModel(app)
    {}

    gui::ListItem *SearchResultsModel::getItem(int index)
    {
        auto ret = new gui::SearchResultsItem();
        ret->setContact("Title: " + std::to_string(index));
        ret->setTimestamp("00:00:0000");
        ret->setPreview("this is preview text");
        ret->setID(index);
        return ret;
    }

}; // namespace gui::model
