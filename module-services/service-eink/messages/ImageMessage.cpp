// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkMessage.hpp"
#include "ImageMessage.hpp"

#include <MessageType.hpp>

namespace seink
{

    ImageMessage::ImageMessage(
        uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool deepRefresh, uint8_t *data, bool suspend, bool shutdown)
        : EinkMessage(MessageType::EinkImageData), x{x}, y{y}, w{w}, h{h},
          deepRefresh{deepRefresh}, data{data}, suspend{suspend}, shutdown{shutdown}
    {}

    ImageMessage::~ImageMessage()
    {}

} /* namespace seink */
