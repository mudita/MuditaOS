// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

class LanguagesData : public gui::SwitchData
{
  public:
    explicit LanguagesData(Language currentDisplayLanguage) : currentDisplayLanguage(std::move(currentDisplayLanguage))
    {}
    [[nodiscard]] auto getCurrentDisplayLanguage() const noexcept -> Language
    {
        return currentDisplayLanguage;
    }

  private:
    Language currentDisplayLanguage;
};
