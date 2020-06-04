#include <application-phonebook/ApplicationPhonebook.hpp>
#include <module-apps/application-phonebook/windows/PhonebookContact.hpp>
#include "SearchResultsModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "UiCommonActions.hpp"
#include "ListView.hpp"

SearchResultsModel::SearchResultsModel(app::Application *app) : application{app}
{}

SearchResultsModel::~SearchResultsModel()
{
    results = nullptr;
}

void SearchResultsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    internalOffset = offset;
    internalLimit  = limit;
    list->onProviderDataUpdate();
}

unsigned int SearchResultsModel::getMinimalItemHeight()
{
    return phonebookStyle::contactItem::h;
}

gui::ListItem *SearchResultsModel::getItem(gui::Order order)
{
    auto index = 0;
    if (order == gui::Order::Previous) {
        index = internalOffset + internalLimit - 1;
    }
    if (order == gui::Order::Next) {
        index = internalOffset;
    }

    if (results != nullptr && index < static_cast<int>(results->size()) && index >= 0) {
        auto contact = std::make_shared<ContactRecord>(results->at(index));

        LOG_DEBUG("index: %d, id: %d, limit: %d, name: %s %s, fav: %d",
                  index,
                  internalOffset,
                  internalLimit,
                  contact->primaryName.c_str(),
                  contact->alternativeName.c_str(),
                  contact->isOnFavourites);

        if (order == gui::Order::Previous) {
            internalOffset--;
        }
        if (order == gui::Order::Next) {
            internalOffset++;
        }

        if (contact == nullptr) {
            return nullptr;
        }

        gui::PhonebookItem *item = new gui::PhonebookItem();

        item->setContact(contact);
        item->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("activatedCallback");
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(gui::window::name::contact, std::move(data));
            return true;
        };

        item->inputCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("calling");
                return app::call(application, *item->contact);
            }
            return false;
        };

        return item;
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

    LOG_INFO("setResults count: %d", static_cast<int>(results.get()->size()));
}
