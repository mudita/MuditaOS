// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalWindowPresenter.hpp"

TechnicalWindowPresenter::TechnicalWindowPresenter(std::shared_ptr<gui::ListItemProvider> technicalInformationProvider)
    : technicalInformationProvider{std::move(technicalInformationProvider)}
{}

std::shared_ptr<gui::ListItemProvider> TechnicalWindowPresenter::getTechnicalInformationProvider() const
{
    return technicalInformationProvider;
}
