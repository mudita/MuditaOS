// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationCall.hpp"
#include "StateIcons.hpp"

#include <AppWindow.hpp>
#include <ContactRecord.hpp>
#include <country.hpp>
#include <gui/input/Translator.hpp>
#include <phonenumbers/asyoutypeformatter.h>
#include <phonenumbers/phonenumberutil.h>

#include <memory>
#include <string>

namespace gui
{
    class Label;
    class NumberWindow : public AppWindow
    {
      public:
        using CountryCode = utils::country::Id;

        NumberWindow(app::ApplicationCommon *app,
                     app::EnterNumberWindowInterface *interface,
                     std::string windowName = app::window::name_number);

        auto onInput(const InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getEnteredNumber() const noexcept -> const std::string &;

        void buildInterface() override;
        void destroyInterface() override;
        void initFormatterInput(const std::string &number);

      protected:
        app::EnterNumberWindowInterface *interface = nullptr;
        gui::Label *numberLabel                    = nullptr;
        gui::Label *numberDescriptionLabel         = nullptr;
        std::string formattedNumber;
        std::string enteredNumber;

        void setNumberLabel(const std::string &num);

        void switchFormatter(const std::string &countryCode);
        void addDigit(const std::string::value_type &digit);
        void clearInput();

      private:
        using PhoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil;
        using Formatter       = i18n::phonenumbers::AsYouTypeFormatter;

        CountryCode defaultCountry = utils::country::defaultCountry;
        CountryCode currentCountry;
        gui::KeyInputMappedTranslation translator;
        PhoneNumberUtil &numberUtil;
        std::unique_ptr<Formatter> formatter;
    };
} /* namespace gui */
