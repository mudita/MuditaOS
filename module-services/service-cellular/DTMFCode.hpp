// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stdint.h>
#include <string>

class DTMFCode
{
  public:
    /// \brief Creates a DTMFCode instance via passing a digit character (its ASCII code)
    /// \param digitChar - digit's ASCII code
    explicit DTMFCode(char digitChar);

    /// \brief Creates a DTMFCode instance via passing a digit C-string
    /// \param digitChar - C-string containing code's digit
    explicit DTMFCode(const char *digitString);

    [[nodiscard]] auto getDigitASCIICode() const -> char;
    operator std::string() const;

  private:
    char digitChar;
};
