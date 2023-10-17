﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <application-phonebook/windows/PhonebookContactDetails.hpp>
#include <AsyncTask.hpp>

#include "ListView.hpp"
#include "PhonebookModel.hpp"

#include <Common/Query.hpp>
#include <queries/phonebook/QueryContactGet.hpp>
#include <queries/RecordQuery.hpp>

#include <service-db/QueryMessage.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-call/data/CallSwitchData.hpp>

#include <string>
#include <utility>

namespace
{
    constexpr auto phonebookModelTimeout = 1000;
}

PhonebookModel::PhonebookModel(app::ApplicationCommon *app) : DatabaseModel(app), app::AsyncCallbackReceiver{app}
{}

auto PhonebookModel::requestRecordsCount() -> unsigned int
{

    auto dispMode = static_cast<ContactDisplayMode>(getDisplayMode());
    if (dispMode == ContactDisplayMode::SortedByLetter) {
        letterMap    = requestLetterMap();
        recordsCount = letterMap.itemCount;
        return recordsCount;
    }

    auto [code, msg] = DBServiceAPI::GetQueryWithReply(
        application,
        db::Interface::Name::Contact,
        std::make_unique<db::query::ContactGetSize>(queryFilter, queryGroupFilter, queryDisplayMode),
        phonebookModelTimeout);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        auto countResultResponse = queryResponse->getResult();
        auto countResult         = dynamic_cast<db::query::RecordsSizeQueryResult *>(countResultResponse.get());
        assert(countResult != nullptr);

        recordsCount = countResult->getSize();
    }

    return recordsCount;
}

void PhonebookModel::requestRecords(const std::uint32_t offset, const std::uint32_t limit)
{
    auto query =
        std::make_unique<db::query::ContactGet>(limit, offset, queryFilter, queryGroupFilter, queryDisplayMode);
    auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
    task->setCallback([this](auto response) { return handleQueryResponse(response); });
    task->execute(application, this);
}

auto PhonebookModel::requestLetterMap() -> ContactsMapData
{

    auto [code, msg] = DBServiceAPI::GetQueryWithReply(
        application,
        db::Interface::Name::Contact,
        std::make_unique<db::query::ContactGetLetterMap>(queryFilter, queryGroupFilter, queryDisplayMode),
        phonebookModelTimeout);

    ContactsMapData contactMapData;
    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        if (queryResponse != nullptr) {
            auto letterMapResultResponse = queryResponse->getResult();
            auto letterMapResult         = dynamic_cast<db::query::LetterMapResult *>(letterMapResultResponse.get());
            if (letterMapResult != nullptr) {
                contactMapData = letterMapResult->getLetterMap();
            }
        }
    }

    return contactMapData;
}

auto PhonebookModel::updateRecords(std::vector<ContactRecord> records) -> bool
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();

    return true;
}

auto PhonebookModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return phonebookStyle::contactItem::h;
}

auto PhonebookModel::getItem(gui::Order order) -> gui::ListItem *
{
    const auto contact = getRecord(order);

    if (contact == nullptr) {
        return nullptr;
    }

    auto item = new gui::PhonebookItem();

    item->setContact(contact);
    item->setLabelMarkerDisplayMode(getLabelMarkerDisplayMode(contact->contactPosOnList));
    item->activatedCallback = [this, item, contact](gui::Item &) {
        if (customContactActivationCallback) {
            return customContactActivationCallback(item);
        }

        application->switchWindow(gui::window::name::contact, std::make_unique<PhonebookItemData>(contact));
        return true;
    };

    item->inputCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
        if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
            if (item->contact && !item->contact->numbers.empty()) {
                const auto phoneNumber = item->contact->numbers.front().number;
                return app::manager::Controller::sendAction(application,
                                                            app::manager::actions::Call,
                                                            std::make_unique<app::ExecuteCallData>(phoneNumber),
                                                            app::manager::OnSwitchBehaviour::RunInBackground);
            }
        }
        return false;
    };

    return item;
}

auto PhonebookModel::getFilter() const -> const std::string &
{
    return queryFilter;
}

void PhonebookModel::setDisplayMode(std::uint32_t displayMode)
{
    queryDisplayMode = displayMode;
}

auto PhonebookModel::getDisplayMode() -> std::uint32_t
{
    return queryDisplayMode;
}

auto PhonebookModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto contactsResponse = dynamic_cast<db::query::ContactGetResult *>(queryResult);
    assert(contactsResponse != nullptr);

    auto records = std::vector<ContactRecord>(contactsResponse->getRecords());

    return this->updateRecords(std::move(records));
}

auto PhonebookModel::getLabelMarkerDisplayMode(uint32_t posOnList) -> LabelMarkerDisplayMode
{
    if (posOnList < letterMap.favouritesCount) {
        return LabelMarkerDisplayMode::IncludeFavourites;
    }
    return LabelMarkerDisplayMode::IgnoreFavourites;
}

void PhonebookModel::setFilter(std::string filter, const std::uint32_t groupFilter, const std::uint32_t displayMode)
{
    queryFilter      = std::move(filter);
    queryGroupFilter = groupFilter;
    queryDisplayMode = displayMode;
}
