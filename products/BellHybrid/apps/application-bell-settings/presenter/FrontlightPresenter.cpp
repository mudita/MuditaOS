// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightPresenter.hpp"

namespace app::bell_settings
{
    FrontlightWindowPresenter::FrontlightWindowPresenter(std::shared_ptr<FrontlightModel> pagesProvider)
        : pagesProvider(std::move(pagesProvider))
    {}

    auto FrontlightWindowPresenter::saveData() -> void
    {
        pagesProvider->saveData();
    }

    auto FrontlightWindowPresenter::loadData() -> void
    {
        pagesProvider->loadData();
    }

    auto FrontlightWindowPresenter::createData() -> void
    {
        pagesProvider->createData();
    }

    auto FrontlightWindowPresenter::getPagesProvider() const -> std::shared_ptr<FrontlightModel>
    {
        return pagesProvider;
    }
} // namespace app::bell_settings
