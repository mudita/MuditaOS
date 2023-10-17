// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultipleNumbersModel.hpp"

#include <application-phonebook/data/PhonebookStyle.hpp>
#include <application-phonebook/data/PhonebookItemData.hpp>
#include <application-phonebook/widgets/MultipleNumbersWidget.hpp>

#include <service-appmgr/Controller.hpp>
#include <ListView.hpp>

MultipleNumbersModel::MultipleNumbersModel(app::ApplicationCommon *app) : application(app)
{}

auto MultipleNumbersModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto MultipleNumbersModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return phonebookStyle::inputLinesWithLabelWidget::h;
}

void MultipleNumbersModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto MultipleNumbersModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void MultipleNumbersModel::createData(std::shared_ptr<ContactRecord> record)
{
    contactRecord = std::move(record);

    auto firstNumber = new gui::MultipleNumbersWidget(utils::translate("app_phonebook_multiple_numbers_first"),
                                                      contactRecord->numbers.at(0).number.getFormatted());

    firstNumber->inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
        if (event.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
            std::unique_ptr<PhonebookSearchRequest> data = std::make_unique<PhonebookSearchRequest>();
            data->result                                 = contactRecord;
            data->selectedNumber                         = 0;
            data->setDescription("PhonebookSearchRequest");

            app::manager::Controller::switchBack(
                application,
                std::make_unique<app::manager::SwitchBackRequest>(application->GetName(), std::move(data)));
            return true;
        }
        return false;
    };

    auto secondNumber = new gui::MultipleNumbersWidget(utils::translate("app_phonebook_multiple_numbers_second"),
                                                       contactRecord->numbers.at(1).number.getFormatted());

    secondNumber->inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
        if (event.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
            std::unique_ptr<PhonebookSearchRequest> data = std::make_unique<PhonebookSearchRequest>();
            data->result                                 = contactRecord;
            data->selectedNumber                         = 1;
            data->setDescription("PhonebookSearchRequest");

            app::manager::Controller::switchBack(
                application,
                std::make_unique<app::manager::SwitchBackRequest>(application->GetName(), std::move(data)));
            return true;
        }
        return false;
    };

    internalData.push_back(firstNumber);
    internalData.push_back(secondNumber);

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}
