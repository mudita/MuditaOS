#include <application-phonebook/ApplicationPhonebook.hpp>
#include <module-apps/application-phonebook/windows/PhonebookContact.hpp>
#include "ListView.hpp"
#include "PhonebookModel.hpp"
#include "i18/i18.hpp"

#include <messages/QueryMessage.hpp>
#include <queries/phonebook/QueryContactGet.hpp>
#include <queries/RecordQuery.hpp>

#include "service-db/api/DBServiceAPI.hpp"
#include "UiCommonActions.hpp"

#include <string>

const static std::uint32_t phonebookModelTimeout = 1000;

PhonebookModel::PhonebookModel(app::Application *app, std::string filter)
    : DatabaseModel(app), queryFilter(std::move(filter))
{
    requestRecordsCount();
}

void PhonebookModel::requestRecordsCount()
{
    auto [code, msg] = DBServiceAPI::GetQueryWithReply(application,
                                                       db::Interface::Name::Contact,
                                                       std::make_unique<db::query::ContactGetSize>(queryFilter),
                                                       phonebookModelTimeout);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        auto countResult = dynamic_cast<db::query::RecordsSizeQueryResult *>(queryResponse->getResult());
        assert(countResult != nullptr);

        recordsCount = countResult->getSize();
    }
}

void PhonebookModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::GetQuery(
        application, db::Interface::Name::Contact, std::make_unique<db::query::ContactGet>(offset, limit, queryFilter));
}

auto PhonebookModel::updateRecords(std::unique_ptr<std::vector<ContactRecord>> records,
                                   const uint32_t offset,
                                   const uint32_t limit,
                                   uint32_t count) -> bool
{

#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %u, Limit: %u Count:%u", offset, limit, count);
    for (uint32_t i = 0; i < records->size(); ++i) {
        LOG_DEBUG("id: %u, name: %s %s, fav: %d",
                  records.get()->operator[](i).ID,
                  records.get()->operator[](i).primaryName.c_str(),
                  records.get()->operator[](i).alternativeName.c_str(),
                  records.get()->operator[](i).isOnFavourites);
    }
#endif

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);
    modelIndex = 0;
    list->onProviderDataUpdate();

    return true;
}

auto PhonebookModel::getMinimalItemHeight() const -> unsigned int
{
    return phonebookStyle::contactItem::h;
}

auto PhonebookModel::getItem(gui::Order order) -> gui::ListItem *
{

    std::shared_ptr<ContactRecord> contact = getRecord(order);

    if (contact == nullptr) {
        return nullptr;
    }

    auto item = new gui::PhonebookItem();

    item->setContact(contact);
    item->activatedCallback = [this, item, contact](gui::Item &) {
        if (messagesSelectCallback && messagesSelectCallback(item)) {
            return true;
        }

        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);

        application->switchWindow(gui::window::name::contact, std::move(data));
        return true;
    };

    item->inputCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
        if (messagesSelectCallback) {
            return false;
        }
        if (event.state != gui::InputEvent::State::keyReleasedShort) {
            return false;
        }
        if (event.keyCode == gui::KeyCode::KEY_LF) {
            return app::call(application, *item->contact);
        }
        return false;
    };

    return item;
}

auto PhonebookModel::getFilter() const -> const std::string &
{
    return queryFilter;
}
