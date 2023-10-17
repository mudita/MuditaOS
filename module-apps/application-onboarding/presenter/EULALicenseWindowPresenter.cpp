// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EULALicenseWindowPresenter.hpp"

namespace app::onBoarding
{
    EULALicenseWindowPresenter::EULALicenseWindowPresenter(Function acceptEULAApp,
                                                           std::unique_ptr<AbstractEULARepository> &&eulaRepository)
        : acceptEULAApp(std::move(acceptEULAApp)), eulaRepository{std::move(eulaRepository)}
    {}

    void EULALicenseWindowPresenter::acceptEULA()
    {
        acceptEULAApp();
    }

    std::string EULALicenseWindowPresenter::getEULA()
    {
        return eulaRepository->getEulaText();
    }
} // namespace app::onBoarding
