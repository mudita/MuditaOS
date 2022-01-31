// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookMultipleNumbersSelect.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <application-phonebook/data/PhonebookItemData.hpp>

namespace gui
{
    PhonebookMultipleNumbersSelect::PhonebookMultipleNumbersSelect(app::ApplicationCommon *app,
                                                                   std::shared_ptr<MultipleNumbersModel> &&numberModel)
        : AppWindow(app, gui::window::name::multiple_numbers_select), numberModel(std::move(numberModel))
    {
        buildInterface();
    }

    void PhonebookMultipleNumbersSelect::buildInterface()
    {
        AppWindow::buildInterface();

        numberList = new gui::ListView(this,
                                       phonebookStyle::mainWindow::contactsList::x,
                                       phonebookStyle::mainWindow::contactsList::y,
                                       phonebookStyle::mainWindow::contactsList::w,
                                       phonebookStyle::mainWindow::contactsList::h,
                                       numberModel);
        numberList->setBoundaries(Boundaries::Continuous);
        setFocusItem(numberList);

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setFocusItem(numberList);
    }

    void PhonebookMultipleNumbersSelect::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto numberSelectData = dynamic_cast<PhonebookMultipleNumbersRequest *>(data);
        assert(numberSelectData);

        setTitle(numberSelectData->record->getFormattedName());
        numberModel->createData(numberSelectData->record);

        numberList->rebuildList();
    }

} // namespace gui
