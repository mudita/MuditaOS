// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractKeySequence.hpp"
#include <keymap/KeyMap.hpp>

class ReleaseSequence : public AbstractKeySequence
{
  public:
    explicit ReleaseSequence(KeyMap key) : mappedKey{key}
    {}
    void process(const RawKey &key) override
    {
        if (mapKey(static_cast<gui::KeyCode>(key.keyCode)) != mappedKey) {
            return;
        }

        if (key.state == RawKey::State::Released) {
            trigger();
            ready();
        }
    }

  private:
    KeyMap mappedKey;
};
