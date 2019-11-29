#include "SearchResultsModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"

SearchResultsModel::SearchResultsModel(app::Application *app) : results(nullptr), favouriteCount(0)
{
}

SearchResultsModel::~SearchResultsModel()
{
    results = nullptr;
}

gui::ListItem *SearchResultsModel::getItem(int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown)
{
    /*
    LOG_INFO("requested item: %d first %d prev %d count %d", index, firstElement, prevIndex, count);
    if (results && index < (int)results->size())
    {
        gui::PhonebookItem *item = new gui::PhonebookItem();
        auto o = *results.get();
        LOG_INFO("item: %s", o[index].primaryName.c_str());
        auto contact = std::make_shared<ContactRecord>(o[index]);
        item->setContact(contact);
        item->setID(index);
        return (item);
    }
    else
    {
        return (nullptr);
    }
    */
    auto o = *results.get();
    auto contact = std::make_shared<ContactRecord>(o[index]);

    if (contact == nullptr)
        return nullptr;

    if (topDown)
    {
        // return item from favorite part of contacts
        if (static_cast<uint32_t>(index) < favouriteCount)
        {
            gui::PhonebookItem *item = new gui::PhonebookItem();

            if ((index == firstElement) && (index != prevIndex))
            {
                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
            }
            else
            {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            return item;
        }
        else
        {
            gui::PhonebookItem *item = new gui::PhonebookItem();
            // on top the page or if element next after last favourite contact is requested
            if (((index == firstElement) || (static_cast<uint32_t>(index) == favouriteCount)) && (index != prevIndex))
            {

                item->setValue(contact->primaryName.substr(0, 1));
            }
            else
            {
                auto prevContact = std::make_shared<ContactRecord>(o[prevIndex]);
                if (contact->alternativeName.substr(0, 1) == prevContact->alternativeName.substr(0, 1))
                {
                    item->setContact(contact);
                    item->setID(index);
                }
                else
                {
                    item->setValue(contact->primaryName.substr(0, 1));
                }
            }
            return item;
        }
    }
    else
    {
        if (static_cast<uint32_t>(index) < favouriteCount - 1)
        {
            gui::PhonebookItem *item = new gui::PhonebookItem();
            if (remaining == 0)
            {
                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
            }
            else
            {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            return item;
        }
        else
        {
            gui::PhonebookItem *item = new gui::PhonebookItem();

            // leaving normal contacts list and entering favourite area but charatcer is already placed
            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index == prevIndex))
            {
                item->markFavourite(true);
                item->setContact(contact);
                item->setID(index);
            }
            // leaving normal contacts list and entering favourite area - return character
            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index != prevIndex))
            {
                item->setValue(contact->primaryName.substr(0, 1));
            }
            else
            {
                auto prevContact = std::make_shared<ContactRecord>(o[prevIndex]);
                if (remaining == 0)
                {
                    // previous element has the same first character of alternative name so display first character
                    if (index == prevIndex)
                    {
                        item->setContact(contact);
                        item->setID(index);
                    }
                    else
                    {
                        item->setValue(prevContact->primaryName.substr(0, 1));
                    }
                }
                else if (((index == firstElement) || (index == prevIndex) || (contact->primaryName.substr(0, 1) == prevContact->primaryName.substr(0, 1))))
                {
                    item->setContact(contact);
                    item->setID(index);
                }
                else
                {
                    item->setValue(prevContact->primaryName.substr(0, 1));
                }
            }
            return item;
        }
    }
    return nullptr;
}

int SearchResultsModel::getItemCount()
{
    if (results)
    {
        return (results.get()->size());
    }
    else
    {
        return (0);
    }
}

void SearchResultsModel::setResults(std::shared_ptr<std::vector<ContactRecord>> _results)
{
    results = _results;

    LOG_INFO("setResults count:%d", results.get()->size());
}
