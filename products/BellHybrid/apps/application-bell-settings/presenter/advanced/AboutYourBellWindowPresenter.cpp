// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellWindowPresenter.hpp"
#include <models/advanced/AboutYourBellModel.hpp>

namespace app::bell_settings
{
    AboutYourBellWindowPresenter::AboutYourBellWindowPresenter(std::shared_ptr<AboutYourBellModel> pagesProvider)
        : pagesProvider(std::move(pagesProvider))
    {}

    auto AboutYourBellWindowPresenter::createData() -> void
    {
        pagesProvider->createData();
    }

    auto AboutYourBellWindowPresenter::getPagesProvider() const -> std::shared_ptr<AboutYourBellModel>
    {
        return pagesProvider;
    }
} // namespace app::bell_settings
