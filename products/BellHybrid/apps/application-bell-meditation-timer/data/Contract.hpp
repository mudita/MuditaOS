// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <widgets/ListItemProvider.hpp>

namespace app::meditation::contract
{
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };

    class Presenter : public BasePresenter<View>
    {
      public:
        virtual ~Presenter() noexcept                                                   = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void loadData()                                                         = 0;
        virtual void saveData()                                                         = 0;
        virtual void eraseProviderData()                                                = 0;
        virtual void handleEnter()                                                      = 0;
    };
} // namespace app::meditation::contract
