// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/BellListItemProvider.hpp>

namespace app::bell_settings
{
    class AbstractFrontlightModel;

    class FrontlightListItemProvider : public app::BellListItemProvider
    {
      public:
        explicit FrontlightListItemProvider(AbstractFrontlightModel &model);

      private:
        void buildListItems();

        AbstractFrontlightModel &model;
    };
} // namespace app::bell_settings
