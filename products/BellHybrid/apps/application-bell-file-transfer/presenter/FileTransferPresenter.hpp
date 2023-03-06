// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app::bell_file_transfer
{
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };

    class AbstractFileTransferPresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractFileTransferPresenter() noexcept = default;
    };

    class FileTransferPresenter : public AbstractFileTransferPresenter
    {
      public:
        FileTransferPresenter(){};
    };
} // namespace app::bell_bedtime
