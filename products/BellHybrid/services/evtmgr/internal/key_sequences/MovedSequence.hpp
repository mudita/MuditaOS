// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractKeySequence.hpp"
#include <keymap/KeyMap.hpp>

class MovedSequence : public AbstractKeySequence
{
  public:
    explicit MovedSequence(KeyMap key) : mappedKey{key}
    {}
    void process(const RawKey &key) override
    {
        if (mapKey(static_cast<gui::KeyCode>(key.keyCode)) != mappedKey) {
            return;
        }

        if (key.state == RawKey::State::Moved) {
            trigger();
            ready();
        }
    }

  private:
    KeyMap mappedKey;
};
