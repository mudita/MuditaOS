// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportWindowPresenter.hpp"

SimContactsImportWindowPresenter::SimContactsImportWindowPresenter(
    app::ApplicationCommon *application, std::shared_ptr<SimContactsImportModel> simContactsProvider)
    : application(application), simContactsProvider{std::move(simContactsProvider)}
{
    onSave = [&]() {
        this->simContactsProvider->clearData();
        getView()->contactsImported();
        this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        requestCompleted = true;
    };

    onDuplicatesCheck = [&](bool duplicatesFound) {
        if (duplicatesFound) {
            duplicatesChecked = true;
            this->simContactsProvider->clearData();
            getView()->displayDuplicatesCount(this->simContactsProvider->getDuplicatesCount());
            this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        else {
            this->simContactsProvider->saveData(onSave);
        }
        requestCompleted = true;
    };

    onSimContactsReady = [&]() {
        this->simContactsProvider->createSimImported();
        getView()->contactsReady();
        this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    };
}

std::shared_ptr<gui::ListItemProvider> SimContactsImportWindowPresenter::getSimContactsProvider() const
{
    return simContactsProvider;
}

void SimContactsImportWindowPresenter::eraseProviderData() const
{
    simContactsProvider->eraseData();
}

void SimContactsImportWindowPresenter::saveImportedContacts()
{
    this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    requestCompleted = false;
    if (!duplicatesChecked) {
        simContactsProvider->findDuplicates(onDuplicatesCheck);
    }
    else {
        simContactsProvider->saveData(onSave);
    }
}

bool SimContactsImportWindowPresenter::isRequestCompleted()
{
    return requestCompleted;
}

void SimContactsImportWindowPresenter::requestSimContacts()
{
    simContactsProvider->requestSimContacts(onSimContactsReady);
}

void SimContactsImportWindowPresenter::requestDuplicates()
{
    this->simContactsProvider->createDuplicates();
    getView()->displayDuplicates();
    this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
}
