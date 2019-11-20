#include "SearchResultsModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"

SearchResultsModel::SearchResultsModel(app::Application *app) : results(nullptr)
{
}

SearchResultsModel::~SearchResultsModel()
{
    results = nullptr;
}

gui::ListItem *SearchResultsModel::getItem(int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown)
{
    if (results && index < (int)results->size())
    {
        return (nullptr);
    }
    else
    {
        return (nullptr);
    }
}

int SearchResultsModel::getItemCount()
{
    if (results)
    {
        return (results->size());
    }
    else
    {
        return (0);
    }
}
