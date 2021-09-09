// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportModel.hpp"

#include <application-settings/widgets/network/SimContactImportSelectWidget.hpp>
#include <ListView.hpp>

SimContactsImportModel::SimContactsImportModel(app::ApplicationCommon *app,
                                               std::unique_ptr<AbstractSimContactsRepository> contactsRepository)
    : application(app), contactsRepository(std::move(contactsRepository))
{}

auto SimContactsImportModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto SimContactsImportModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return style::window::label::big_h + style::margins::big;
}

void SimContactsImportModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto SimContactsImportModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void SimContactsImportModel::createSimImported()
{
    createData(contactsRepository->getImportedRecords());
}

void SimContactsImportModel::createDuplicates()
{
    createData(contactsRepository->getDuplicatedRecords());
}

unsigned int SimContactsImportModel::getDuplicatesCount()
{
    return contactsRepository->getDuplicatedRecords().size();
}

void SimContactsImportModel::createData(const std::vector<ContactRecord> &importedRecords)
{
    auto app = application;

    for (const auto &record : importedRecords) {
        internalData.push_back(new gui::SimContactImportSelectWidget(
            record.primaryName + " " + record.alternativeName,
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void SimContactsImportModel::clearData()
{
    list->reset();
    eraseInternalData();
}

void SimContactsImportModel::eraseData()
{
    clearData();
    contactsRepository->clear();
}

std::vector<bool> SimContactsImportModel::getSelectedContacts()
{
    std::vector<bool> selectedContacts;
    for (const auto &item : internalData) {
        selectedContacts.push_back(item->isChecked());
    }
    return selectedContacts;
}

void SimContactsImportModel::findDuplicates(std::function<void(bool)> onDuplicatesCheckCallback)
{
    contactsRepository->findDuplicates(getSelectedContacts(), std::move(onDuplicatesCheckCallback));
}

void SimContactsImportModel::saveData(std::function<void()> onSaveCallback)
{
    auto duplicatesFound = getDuplicatesCount() != 0;
    contactsRepository->save(getSelectedContacts(), duplicatesFound, std::move(onSaveCallback));
}

void SimContactsImportModel::requestSimContacts(std::function<void()> onSimContactsReadCallback)
{
    contactsRepository->read(std::move(onSimContactsReadCallback));
}
