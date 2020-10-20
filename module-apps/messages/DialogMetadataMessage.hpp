// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <windows/DialogMetadata.hpp>
#include <memory>

namespace gui
{
    class DialogMetadataMessage : public gui::SwitchData
    {
        DialogMetadata metadata;

      public:
        DialogMetadataMessage(DialogMetadata metadata) : metadata(metadata)
        {}
        [[nodiscard]] const DialogMetadata &get() const
        {
            return metadata;
        }
    };
} // namespace gui
