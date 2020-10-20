// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <ImageBox.hpp>
#include <TextFixedSize.hpp>
#include <module-utils/PhoneNumber.hpp>

namespace gui
{
    class NumberWithIconsWidget : public HBox
    {
      public:
        NumberWithIconsWidget(app::Application *app,
                              const utils::PhoneNumber::View &number,
                              const std::string &font,
                              Item *parent);
        ~NumberWithIconsWidget() override = default;

        TextFixedSize *numberText = nullptr;
        ImageBox *phoneImage      = nullptr;
        ImageBox *smsImage        = nullptr;
    };

} /* namespace gui */
