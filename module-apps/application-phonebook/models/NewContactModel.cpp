#include "NewContactModel.hpp"

#include "application-phonebook/widgets/ContactListItem.hpp"
#include "application-phonebook/widgets/InputBoxWithLabelAndIconItem.hpp"
#include "application-phonebook/widgets/InputLineWithLabelItem.hpp"

#include <ListView.hpp>
#include <time/ScopedTime.hpp>

NewContactModel::NewContactModel(app::Application *app) : application(app)
{
    createData();
}

auto NewContactModel::getItemCount() const -> int
{
    return internalData.size();
}

auto NewContactModel::getMinimalItemHeight() const -> unsigned int
{
    return phonebookStyle::inputLineWithLabelItem::h;
}

void NewContactModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto NewContactModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void NewContactModel::createData()
{
    auto app = application;

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::FirstName,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::SecondName,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::Number,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::OtherNumber,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::Email,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputBoxWithLabelAndIconItem(phonebookInternals::ListItemName::SpeedDialKey));

    internalData.push_back(new gui::InputBoxWithLabelAndIconItem(
        phonebookInternals::ListItemName::AddToFavourites,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

    internalData.push_back(new gui::InputBoxWithLabelAndIconItem(
        phonebookInternals::ListItemName::AddToICE,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::Address,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    internalData.push_back(new gui::InputLineWithLabelItem(
        phonebookInternals::ListItemName::Note,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); }));

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void NewContactModel::clearData()
{
    list->clear();

    eraseInternalData();

    createData();

    requestRecords(0, internalData.size());
}

void NewContactModel::saveData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (auto item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(contactRecord);
        }
    }
}

void NewContactModel::loadData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (auto item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(contactRecord);
        }
    }
}
