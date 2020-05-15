/*
 * @file PhonebookModel.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 10 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PhonebookModel.hpp"
#include "i18/i18.hpp"

#include "../widgets/PhonebookItem.hpp"
#include "service-db/api/DBServiceAPI.hpp"

PhonebookModel::PhonebookModel(app::Application *app) : DatabaseModel(app)
{}

void PhonebookModel::requestRecordsCount()
{

    favouriteCount = DBServiceAPI::ContactGetCount(application, true);

    recordsCount = DBServiceAPI::ContactGetCount(application);

    LOG_DEBUG("Fav: %d, Contact: %d", favouriteCount, recordsCount);

    DBServiceAPI::ContactGetLimitOffset(application, 0, phonebookStyle::mainWindow::contactsList::pageSize);
}

void PhonebookModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    LOG_DEBUG("Req off: %d, limit: %d", offset, limit);

    DBServiceAPI::ContactGetLimitOffset(application, offset, limit);
}

bool PhonebookModel::updateRecords(std::unique_ptr<std::vector<ContactRecord>> records,
                                   const uint32_t offset,
                                   const uint32_t limit,
                                   uint32_t count)
{

#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %d, Limit: %d Count:%d", offset, limit, count);
    for (uint32_t i = 0; i < records.get()->size(); ++i) {
        LOG_DEBUG("id: %d, name: %s %s, fav: %d",
                  records.get()->operator[](i).ID,
                  records.get()->operator[](i).primaryName.c_str(),
                  records.get()->operator[](i).alternativeName.c_str(),
                  records.get()->operator[](i).isOnFavourites);
    }
#endif

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);
    preProcessData(offset, limit);
    list->onProviderDataUpdate();

    return true;
}

void PhonebookModel::preProcessData(const uint32_t offset, const uint32_t limit)
{
    if (offset < favouriteCount) {
        records.insert(records.begin(),
                       std::make_shared<ContactRecord>(ContactRecord{.note = "fav", .isEmptyListLabel = true}));
    }
}

gui::ListItem *PhonebookModel::getItem(int index)
{
    std::shared_ptr<ContactRecord> contact = getRecord(index);

    if (contact == nullptr) {
        return nullptr;
    }

    gui::PhonebookItem *item = new gui::PhonebookItem();

    if (contact->isEmptyListLabel) {
        if (contact->note == "fav") {
            item->setValue(utils::localize.get("app_phonebook_list_favourites"));
        }
        else {
            item->setValue(contact->note);
        }
        item->activeItem = false;
    }
    else {
        item->setContact(contact);
        item->setContact(contact);
        item->setID(index);
    }

    return item;

    //    auto compareFirstChar = [&](std::shared_ptr<ContactRecord> contact, std::shared_ptr<ContactRecord>
    //    prevContact) {
    //        if (contact->primaryName.substr(0, 1) == prevContact->primaryName.substr(0, 1)) {
    //            return true;
    //        }
    //        else {
    //            return false;
    //        }
    //    };

    //    bool download = false;
    //    if (index > firstIndex + pageSize / 2)
    //        download = true;
    //    if (index < firstIndex - pageSize / 2)
    //        download = true;
    //    std::shared_ptr<ContactRecord> contact = getRecord(index, download);
    //
    //    if (contact == nullptr)
    //        return nullptr;
    //
    //    if (topDown) {
    //        // return item from favorite part of contacts
    //        if (static_cast<uint32_t>(index) < favouriteCount) {
    //            gui::PhonebookItem *item = new gui::PhonebookItem();
    //
    //            if ((index == firstElement) && (index != prevIndex)) {
    //                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
    //            }
    //            else {
    //                item->markFavourite(true);
    //                item->setContact(contact);
    //                item->setID(index);
    //            }
    //            return item;
    //        }
    //        else {
    //            gui::PhonebookItem *item = new gui::PhonebookItem();
    //            // on top the page or if element next after last favourite contact is requested
    //            if (((index == firstElement) || (static_cast<uint32_t>(index) == favouriteCount)) && (index !=
    //            prevIndex)) {
    //
    //                item->setValue(contact->primaryName.substr(0, 1));
    //            }
    //            else {
    //                std::shared_ptr<ContactRecord> prevContact = getRecord(prevIndex, false);
    //                if (compareFirstChar(contact, prevContact)) {
    //                    item->markFavourite(false);
    //                    item->setContact(contact);
    //                    item->setID(index);
    //                }
    //                else {
    //                    item->setValue(contact->primaryName.substr(0, 1));
    //                }
    //            }
    //            return item;
    //        }
    //    }
    //    else {
    //        if (static_cast<uint32_t>(index) < favouriteCount - 1) {
    //            gui::PhonebookItem *item = new gui::PhonebookItem();
    //            if (remaining == 0) {
    //                item->setValue(utils::localize.get("app_phonebook_list_favourites"));
    //            }
    //            else {
    //                item->markFavourite(true);
    //                item->setContact(contact);
    //                item->setID(index);
    //            }
    //            return item;
    //        }
    //        else {
    //            gui::PhonebookItem *item = new gui::PhonebookItem();
    //
    //            // leaving normal contacts list and entering favourite area but character is already placed
    //            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index == prevIndex)) {
    //                item->markFavourite(true);
    //                item->setContact(contact);
    //                item->setID(index);
    //            }
    //            // leaving normal contacts list and entering favourite area - return character
    //            if ((static_cast<uint32_t>(index) == favouriteCount - 1) && (index != prevIndex)) {
    //                item->setValue(contact->primaryName.substr(0, 1));
    //            }
    //            else {
    //                std::shared_ptr<ContactRecord> prevContact = getRecord(prevIndex, false);
    //                if (remaining == 0) {
    //                    // previous element has the same first character of alternative name so display first
    //                    character if (index == prevIndex) {
    //                        item->markFavourite(false);
    //                        item->setContact(contact);
    //                        item->setID(index);
    //                    }
    //                    else {
    //                        item->setValue(prevContact->primaryName.substr(0, 1));
    //                    }
    //                }
    //                else if (((index == firstElement) || (index == prevIndex) ||
    //                          (compareFirstChar(contact, prevContact)))) {
    //                    item->markFavourite(false);
    //                    item->setContact(contact);
    //                    item->setID(index);
    //                }
    //                else {
    //                    item->setValue(prevContact->primaryName.substr(0, 1));
    //                }
    //            }
    //            return item;
    //        }
    //    }
}
