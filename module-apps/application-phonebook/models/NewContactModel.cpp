#include <module-apps/application-phonebook/widgets/InputLineWithLabelItem.hpp>
#include <module-apps/application-phonebook/widgets/InputBoxWithLabelAndIconItem.hpp>
#include <module-apps/application-phonebook/widgets/ContactListItem.hpp>
#include "NewContactModel.hpp"
#include "../data/PhonebookStyle.hpp"
#include "ListView.hpp"

NewContactModel::NewContactModel(app::Application *app)
{
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

NewContactModel::~NewContactModel()
{
    for (auto item : internalData) {
        delete item;
    }
    internalData.clear();
}

auto NewContactModel::getItemCount() const -> int
{
    return internalData.size();
}

auto NewContactModel::getMinimalItemHeight() -> unsigned int
{
    return phonebookStyle::inputLineWithLabelItem::h;
}

void NewContactModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    modelIndex     = 0;
    internalOffset = offset;
    internalLimit  = limit;
    list->onProviderDataUpdate();
}

auto NewContactModel::getItem(gui::Order order) -> gui::ListItem *
{
    unsigned int index = 0;
    if (order == gui::Order::Previous) {
        index = internalLimit - modelIndex - 1;
    }
    if (order == gui::Order::Next) {
        index = internalOffset + modelIndex;
    }

    if (index < internalData.size()) {

        if (order == gui::Order::Previous && index < internalOffset) {
            return nullptr;
        }

        modelIndex++;
        internalData[index]->setVisible(true);
        return internalData[index];
    }

    return nullptr;
}

void NewContactModel::clearData()
{
    auto emptyContact = make_shared<ContactRecord>();
    emptyContact->numbers.emplace_back();
    emptyContact->numbers.emplace_back();
    loadData(emptyContact);
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
