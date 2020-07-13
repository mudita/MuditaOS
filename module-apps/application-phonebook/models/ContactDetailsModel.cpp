#include "ContactDetailsModel.hpp"

#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListView.hpp>
#include <time/ScopedTime.hpp>

ContactDetailsModel::ContactDetailsModel(app::Application *app) : application(app)
{
    createData();
}

auto ContactDetailsModel::getItemCount() const -> int
{
    return internalData.size();
}

auto ContactDetailsModel::getMinimalItemHeight() const -> unsigned int
{
    return phonebookStyle::inputLineWithLabelItem::h;
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
    //auto app = application;

    if(showInformationWidget){
        //internalData.push_back(InformationWidget)
    }

    if(showAddressWidget){
        //internalData.push_back(AddressWidget)
    }

    if(showNoteWidget){
        //internalData.push_back(NoteWidget)
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void ContactDetailsModel::clearData()
{
    list->clear();

    eraseInternalData();

    createData();

    requestRecords(0, internalData.size());
}

void ContactDetailsModel::loadData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (auto item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(contactRecord);
        }
    }
}
