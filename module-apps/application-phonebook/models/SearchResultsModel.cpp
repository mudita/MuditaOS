#include <module-apps/application-phonebook/windows/PhonebookContact.hpp>
#include "SearchResultsModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "UiCommonActions.hpp"

SearchResultsModel::SearchResultsModel(app::Application *app) : DatabaseModel(app)
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
{
    internalOffset = offset;
    list->onProviderDataUpdate();
}

gui::ListItem *SearchResultsModel::getItem(int index)
{
    if (results != nullptr && internalOffset + index < results->size()) {
        auto contact = std::make_shared<ContactRecord>(results->at(internalOffset + index));
        //    std::shared_ptr<ContactRecord> contact = getRecord(index);

        if (contact == nullptr) {
            return nullptr;
        }

        gui::PhonebookItem *item = new gui::PhonebookItem();

        if (item != nullptr) {
            item->setContact(contact);
            item->setID(index);
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
        }

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
