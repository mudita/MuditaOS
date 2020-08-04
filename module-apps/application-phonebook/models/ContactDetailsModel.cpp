#include "ContactDetailsModel.hpp"

#include "application-phonebook/widgets/ContactListItem.hpp"
#include "application-phonebook/widgets/InformationWidget.hpp"
#include "application-phonebook/widgets/MultiLineTextWithLabelItem.hpp"

#include <ListView.hpp>
#include <time/ScopedTime.hpp>
#include <module-apps/application-phonebook/data/PhonebookInternals.hpp>

ContactDetailsModel::ContactDetailsModel(app::Application *app) : application(app)
{}

auto ContactDetailsModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto ContactDetailsModel::getMinimalItemHeight() const -> unsigned int
{
    return phonebookStyle::inputLinesWithLabelItem::h;
}

void ContactDetailsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto ContactDetailsModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void ContactDetailsModel::createData(bool showInformationWidget, bool showAddressWidget, bool showNoteWidget)
{
    if (showInformationWidget) {
        internalData.push_back(new gui::InformationWidget(application));
    }

    if (showAddressWidget) {
        internalData.push_back(new gui::MultiLineTextWithLabelItem(phonebookInternals::ListItemName::Address));
    }

    if (showNoteWidget) {
        internalData.push_back(new gui::MultiLineTextWithLabelItem(phonebookInternals::ListItemName::Note));
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void ContactDetailsModel::loadData(std::shared_ptr<ContactRecord> contactRecord)
{
    list->clear();
    eraseInternalData();

    auto isInformationDataExist = [&]() -> bool {
        return contactRecord->numbers.size() > 0 || contactRecord->mail.length() > 0;
    };
    auto isAddressDataExist = [&]() -> bool { return contactRecord->address.length() > 0; };
    auto isNoteDataExist    = [&]() -> bool { return contactRecord->note.length() > 0; };

    createData(isInformationDataExist(), isAddressDataExist(), isNoteDataExist());

    for (auto item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(contactRecord);
        }
    }

    requestRecords(0, internalData.size());
}
