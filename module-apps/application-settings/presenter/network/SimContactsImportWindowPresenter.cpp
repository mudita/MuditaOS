// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportWindowPresenter.hpp"

SimContactsImportWindowPresenter::SimContactsImportWindowPresenter(
    std::shared_ptr<SimContactsImportModel> simContactsProvider)
    : simContactsProvider{std::move(simContactsProvider)}
{}

std::shared_ptr<gui::ListItemProvider> SimContactsImportWindowPresenter::getSimContactsProvider() const
{
    return simContactsProvider;
}

void SimContactsImportWindowPresenter::clearProviderData() const
{
    simContactsProvider->clearData();
}
