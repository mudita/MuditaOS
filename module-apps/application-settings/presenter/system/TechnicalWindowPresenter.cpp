// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalWindowPresenter.hpp"

TechnicalWindowPresenter::TechnicalWindowPresenter(
    app::ApplicationCommon *application, std::shared_ptr<TechnicalInformationModel> technicalInformationProvider)
    : application(application), technicalInformationProvider{std::move(technicalInformationProvider)}
{
    onImeiReady = [&]() {
        this->technicalInformationProvider->clearData();
        this->technicalInformationProvider->createData();
        getView()->imeiReady();
        this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    };
}

std::shared_ptr<gui::ListItemProvider> TechnicalWindowPresenter::getTechnicalInformationProvider() const
{
    return technicalInformationProvider;
}

void TechnicalWindowPresenter::requestImei()
{
    technicalInformationProvider->requestImei(onImeiReady);
}
