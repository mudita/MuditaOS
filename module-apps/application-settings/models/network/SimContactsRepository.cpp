// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsRepository.hpp"

#include <queries/phonebook/QueryMergeContactsList.hpp>
#include <queries/phonebook/QueryCheckContactsListDuplicates.hpp>
#include <service-cellular/ServiceCellular.hpp>

#include <application-settings/ApplicationSettings.hpp>

SimContactsRepository::SimContactsRepository(app::Application *application)
    : app::AsyncCallbackReceiver{application}, application{application}
{}

const std::vector<ContactRecord> &SimContactsRepository::getImportedRecords()
{
    return importedRecords;
}

const std::vector<ContactRecord> &SimContactsRepository::getDuplicatedRecords()
{
    return duplicatedRecords;
}

void SimContactsRepository::clear()
{
    importedRecords.clear();
    uniqueRecords.clear();
    duplicatedRecords.clear();
}

void SimContactsRepository::findDuplicates(const std::vector<bool> &selectedContacts,
                                           AbstractSimContactsRepository::OnDupplicatesCheckCallback callback)
{
    std::vector<ContactRecord> recordsToSave;
    for (unsigned int i = 0; i < selectedContacts.size(); i++) {
        if (selectedContacts[i]) {
            recordsToSave.push_back(importedRecords[i]);
        }
    }

#if DEBUG_SIM_IMPORT_DATA == 1
    printRecordsData("Sim import selected", recordsToSave);
#endif

    auto query = std::make_unique<db::query::CheckContactsListDuplicates>(recordsToSave);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
    task->setCallback([&, callback](auto response) {
        auto result = dynamic_cast<db::query::CheckContactsListDuplicatesResult *>(response);
        if (result == nullptr) {
            return false;
        }

        auto duplicatesFound = !result->getDuplicates().empty();

        if (callback) {
            if (duplicatesFound) {
                uniqueRecords     = std::move(result->getUnique());
                duplicatedRecords = std::move(result->getDuplicates());

#if DEBUG_SIM_IMPORT_DATA == 1
                printRecordsData("Sim import uniques", uniqueRecords);
                printRecordsData("Sim import duplicates", duplicatedRecords);
#endif
            }
            callback(duplicatesFound);
        }
        return true;
    });
    task->execute(application, this);
}

void SimContactsRepository::save(const std::vector<bool> &selectedContacts,
                                 bool duplicatesFound,
                                 OnSaveCallback callback)
{
    std::vector<ContactRecord> recordsToSave = uniqueRecords;
    for (unsigned int i = 0; i < selectedContacts.size(); i++) {
        if (selectedContacts[i]) {
            recordsToSave.push_back(duplicatesFound ? duplicatedRecords[i] : importedRecords[i]);
        }
    }

#if DEBUG_SIM_IMPORT_DATA == 1
    printRecordsData("Sim import to save data", recordsToSave);
#endif

    auto query = std::make_unique<db::query::MergeContactsList>(recordsToSave);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
    task->setCallback([&, callback](auto response) {
        auto result = dynamic_cast<db::query::MergeContactsListResult *>(response);
        if (result == nullptr) {
            return false;
        }
        if (callback) {
            callback();
        }
        return true;
    });
    task->execute(application, this);
}

void SimContactsRepository::read(AbstractSimContactsRepository::OnReadCallback readDoneCallback)
{
    std::function<void(const std::vector<cellular::SimContact> &simData)> callback =
        [&](const std::vector<cellular::SimContact> &simData) { updateImportedRecords(simData); };

    auto msg  = std::make_unique<cellular::GetSimContactsRequest>();
    auto task = app::AsyncRequest::createFromMessage(std::move(msg), cellular::service::name);
    auto cb   = [callback, readDoneCallback](auto response) {
        auto result = dynamic_cast<cellular::GetSimContactsResponse *>(response);
        if (result != nullptr && result->retCode == sys::ReturnCodes::Success) {
            callback(*result->getContacts());
        }
        readDoneCallback();
        return true;
    };
    task->execute(this->application, this, cb);
}

void SimContactsRepository::updateImportedRecords(const std::vector<cellular::SimContact> &simData)
{
    for (const auto &simRecord : simData) {
        ContactRecord rec = ContactRecord();

        auto description = simRecord.name;
        auto splitPos    = simRecord.name.find(' ');

        rec.primaryName = description.substr(0, splitPos);
        rec.alternativeName =
            splitPos != std::string::npos ? description.substr(rec.primaryName.length() + 1, description.length()) : "";
        rec.numbers.push_back(
            ContactRecord::Number(utils::PhoneNumber(simRecord.number, utils::country::Id::UNKNOWN).getView()));

        importedRecords.push_back(rec);
    }

#if DEBUG_SIM_IMPORT_DATA == 1
    printRecordsData("Imported records from sim", importedRecords);
#endif
}

#if DEBUG_SIM_IMPORT_DATA == 1
void SimContactsRepository::printRecordsData(const std::string &name, const std::vector<ContactRecord> &data)
{
    for (auto record : data) {
        LOG_SENSITIVE("%s: %s %s, Number: %s",
                      name.c_str(),
                      record.primaryName.c_str(),
                      record.alternativeName.c_str(),
                      record.numbers.front().number.getFormatted().c_str());
    }
}
#endif
