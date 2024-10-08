// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
