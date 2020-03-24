#include "SearchResultsModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"

SearchResultsModel::SearchResultsModel(app::Application *app) : DatabaseModel(app, 14)
{}

SearchResultsModel::~SearchResultsModel()
{
    results = nullptr;
}

void SearchResultsModel::requestFavouritesCount()
{
    favouriteCount = DBServiceAPI::ContactGetCount(application, true);
}

void SearchResultsModel::requestRecordsCount()
{}
void SearchResultsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{}

gui::ListItem *SearchResultsModel::getItem(
    int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown)
{
    if (index < 0)
    {
        LOG_ERROR("getItem error: index < 0");
        return nullptr;
    }

    auto searchResults = *results;
    auto itemCount = getItemCount();

    if (index >= itemCount) {
        LOG_ERROR("getItem error: index %d >= results size %d", index, itemCount);
        return nullptr;
    }
    auto contact = std::make_shared<ContactRecord>(searchResults[index]);

    if (contact == nullptr)
        return nullptr;

    if (topDown) {
        // return item from favorite part of contacts
        if (static_cast<uint32_t>(index) < favouriteCount) {
            gui::PhonebookItem *item = new gui::PhonebookItem();

            if ((index == firstElement) && (index != prevIndex)) {
                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
            }
            else {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            return item;
        }
        else {
            gui::PhonebookItem *item = new gui::PhonebookItem();
            // on top the page or if element next after last favourite contact is requested
            if (((index == firstElement) || (static_cast<uint32_t>(index) == favouriteCount)) && (index != prevIndex)) {

                item->setValue(contact->primaryName.substr(0, 1));
            }
            else {
                if (prevIndex < 0)
                {
                    LOG_ERROR("getItem error: prevIndex < 0");
                    return nullptr;
                }
                auto prevContact = std::make_shared<ContactRecord>(searchResults[prevIndex]);
                if (prevContact == nullptr)
                    return nullptr;
                if (contact->alternativeName.substr(0, 1) == prevContact->alternativeName.substr(0, 1)) {
                    item->markFavourite(false);
                    item->setContact(contact);
                    item->setID(index);
                }
                else {
                    item->setValue(contact->primaryName.substr(0, 1));
                }
            }
            return item;
        }
    }
    else {
        if (static_cast<uint32_t>(index) < favouriteCount - 1) {
            gui::PhonebookItem *item = new gui::PhonebookItem();
            if (remaining == 0) {
                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
            }
            else {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            return item;
        }
        else {
            gui::PhonebookItem *item = new gui::PhonebookItem();

            // leaving normal contacts list and entering favourite area but character is already placed
            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index == prevIndex)) {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            // leaving normal contacts list and entering favourite area - return character
            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index != prevIndex)) {
                item->setValue(contact->primaryName.substr(0, 1));
            }
            else {
                if(prevIndex < 0)
                {
                    prevIndex = index-1;
                }
                auto prevContact = std::make_shared<ContactRecord>(searchResults[prevIndex]);
                if (remaining == 0) {
                    // previous element has the same first character of alternative name so display first character
                    if (index == prevIndex) {
                        item->markFavourite(false);
                        item->setContact(contact);
                        item->setID(index);
                    }
                    else {
                        item->setValue(prevContact->primaryName.substr(0, 1));
                    }
                }
                else if (((index == firstElement) || (index == prevIndex) ||
                          (contact->primaryName.substr(0, 1) == prevContact->primaryName.substr(0, 1)))) {
                    item->markFavourite(false);
                    item->setContact(contact);
                    item->setID(index);
                }
                else {
                    item->setValue(prevContact->primaryName.substr(0, 1));
                }
            }
            return item;
        }
    }
    return nullptr;
}

int SearchResultsModel::getItemCount() const
{
    if (results) {
        return (results->size());
    }
    else {
        return (0);
    }
}

void SearchResultsModel::setResults(std::shared_ptr<std::vector<ContactRecord>> _results)
{
    results = _results;

    LOG_INFO("setResults count:%d", results.get()->size());
}
