#include <module-apps/application-phonebook/windows/PhonebookContact.hpp>
#include "ListView.hpp"
#include "PhonebookModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "UiCommonActions.hpp"

PhonebookModel::PhonebookModel(app::Application *app) : DatabaseModel(app)
{}

void PhonebookModel::requestRecordsCount()
{

    favouriteCount = DBServiceAPI::ContactGetCount(application, true);

    recordsCount = DBServiceAPI::ContactGetCount(application);

    DBServiceAPI::ContactGetLimitOffset(application, 0, phonebookStyle::mainWindow::contactsList::pageSize);
}

void PhonebookModel::requestRecords(const uint32_t offset, const uint32_t limit)
{

    DBServiceAPI::ContactGetLimitOffset(application, offset, limit);
}

bool PhonebookModel::updateRecords(std::unique_ptr<std::vector<ContactRecord>> records,
                                   const uint32_t offset,
                                   const uint32_t limit,
                                   uint32_t count)
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

unsigned int PhonebookModel::getMinimalItemHeight()
{
    return phonebookStyle::contactItem::h;
}

gui::ListItem *PhonebookModel::getItem(gui::Order order)
{

    std::shared_ptr<ContactRecord> contact = getRecord(order);

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
