#include "MeditationListItemProviders.hpp"
#include "MeditationListItems.hpp"

#include "ListView.hpp"
#include <algorithm>

#include "application-meditation/windows/Names.hpp"
#include "application-meditation/ApplicationMeditation.hpp"

#include <cassert>

using namespace gui;
uint32_t MeditationListRecord::nextId = 1;

MeditationProvider::MeditationProvider(app::Application *app) : DatabaseModel(app)
{}

bool MeditationProvider::updateRecords(std::vector<MeditationListRecord> newdbRecords)
{
    DatabaseModel::updateRecords(std::move(newdbRecords));
    ListItemProvider::list->onProviderDataUpdate();
    return true;
}

unsigned int MeditationProvider::requestRecordsCount()
{
    return dbRecords->size();
}

unsigned int MeditationProvider::getMinimalItemHeight() const
{
    return style::meditation::item::text::h;
}

void MeditationProvider::requestRecords(const uint32_t offset, const uint32_t limit)
{
    auto dbRecordsCopy = std::vector<MeditationListRecord>();
    for (uint32_t i = offset; i < std::min<uint32_t>(offset + limit, dbRecords->size()); i++) {
        dbRecordsCopy.push_back(MeditationListRecord(dbRecords->at(i)));
    }
    updateRecords(std::move(dbRecordsCopy));
}

PrepTimeProvider::PrepTimeProvider(app::Application *app) : MeditationProvider(app)
{
    dbRecords = std::make_unique<std::vector<MeditationListRecord>>();

    dbRecords->push_back(PreparationTimeRecord(5));
    dbRecords->push_back(PreparationTimeRecord(10));
    dbRecords->push_back(PreparationTimeRecord(30));
    dbRecords->push_back(PreparationTimeRecord(60));
    dbRecords->push_back(PreparationTimeRecord(2 * 60));
    dbRecords->push_back(PreparationTimeRecord(5 * 60));
    dbRecords->push_back(PreparationTimeRecord(10 * 60));
    dbRecords->push_back(PreparationTimeRecord(15 * 60));
    dbRecords->push_back(PreparationTimeRecord(30 * 60));
    dbRecords->push_back(PreparationTimeRecord(60 * 60));
}
#include <iostream>
gui::ListItem *PrepTimeProvider::getItem(gui::Order order)
{
    std::shared_ptr<MeditationListRecord> record = getRecord(order);
    if (record == nullptr) {
        return nullptr;
    }

    auto app = reinterpret_cast<app::ApplicationMeditation *>(application);
    assert(app);

    auto item = new PrepTimeItem(record->getTextValue());
    if (app->state->preparationRecordIndex == record->getId()) {
        item->select(true);
        itemCurrentlySelected              = item;
        app->state->preparationRecordIndex = record->getValue();
    }

    item->activatedCallback = [this, app, item, record](gui::Item &) {
        LOG_DEBUG("Selecting meditation time period");
        if (itemCurrentlySelected != nullptr) {
            itemCurrentlySelected->select(false);
        }
        item->select(true);
        itemCurrentlySelected = item;

        app->state->preparationTimeInSeconds = record->getValue();
        app->state->preparationRecordIndex   = record->getId();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        return true;
    };
    return item;
}

void PrepTimeProvider::requestRecords(const uint32_t offset, const uint32_t limit)
{
    itemCurrentlySelected = nullptr; // new set of ListItems is in use, itemCurrentlySelected ptr should be invalidated
    MeditationProvider::requestRecords(offset, limit);
}

MeditationOptionsProvider::MeditationOptionsProvider(app::Application *app) : MeditationProvider(app)
{
    dbRecords = std::make_unique<std::vector<MeditationListRecord>>();
    dbRecords->push_back(MeditationOptionRecord(MeditationListRecord::Type::Option1));
    dbRecords->push_back(MeditationOptionRecord(MeditationListRecord::Type::Option2));
}

gui::ListItem *MeditationOptionsProvider::getItem(gui::Order order)
{
    std::shared_ptr<MeditationListRecord> record = getRecord(order);
    if (record == nullptr) {
        return nullptr;
    }
    MeditationListItem *item = nullptr;
    if (record->getType() == MeditationListRecord::Type::Option1) {
        item     = new OptionItem1(record->getTextValue());
        auto app = reinterpret_cast<app::ApplicationMeditation *>(application);
        assert(app);
        item->activatedCallback = [this, app, item](gui::Item &) {
            LOG_DEBUG("Selecting OptionItem1");
            app->state->showCounter = !app->state->showCounter;
            item->select(app->state->showCounter);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
    }
    else if (record->getType() == MeditationListRecord::Type::Option2) {
        item                    = new OptionItem2(record->getTextValue());
        item->activatedCallback = [this](gui::Item &) {
            LOG_DEBUG("Selecting OptionItem2");
            application->switchWindow(app::window::name::meditation_preparation);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
    }
    return item;
}

void MeditationOptionsProvider::requestRecords(const uint32_t offset, const uint32_t limit)
{
    MeditationProvider::requestRecords(offset, limit);
}
