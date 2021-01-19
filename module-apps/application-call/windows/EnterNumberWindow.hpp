// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-call/ApplicationCall.hpp"
#include "application-call/widgets/StateIcons.hpp"

#include <Application.hpp>
#include <AppWindow.hpp>
#include <ContactRecord.hpp>
#include <country.hpp>
#include <gui/input/Translator.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/TopBar.hpp>
#include <gui/widgets/Window.hpp>

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>

#include <memory>
#include <string>

namespace gui
{
    class EnterNumberWindow : public AppWindow
    {
      public:
        using CountryCode = utils::country::Id;

      private:
        using PhoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil;
        using Formatter       = i18n::phonenumbers::AsYouTypeFormatter;

      protected:
        app::EnterNumberWindowInterface *interface = nullptr;
        gui::Label *numberLabel                    = nullptr;
        gui::AddContactIcon *newContactIcon        = nullptr;

        void setNumberLabel(const std::string num);

      public:
        EnterNumberWindow(app::Application *app,
                          app::EnterNumberWindowInterface *interface,
                          std::string windowName = app::window::name_enterNumber);
        ~EnterNumberWindow() override = default;

        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        [[nodiscard]] auto getEnteredNumber() const noexcept -> const std::string &;

        void buildInterface() override;
        void destroyInterface() override;

      private:
        CountryCode defaultCountry = utils::country::defaultCountry;
        CountryCode currentCountry;
        gui::KeyInputMappedTranslation translator;
        PhoneNumberUtil &numberUtil;
        std::unique_ptr<Formatter> formatter;
        std::string formattedNumber;
        std::string enteredNumber;

        auto addNewContact() -> bool;
        void switchFormatter(const std::string &countryCode);
        void initFormatterInput(const std::string &number);
        void addDigit(const std::string::value_type &digit);
        void clearInput();
    };

} /* namespace gui */
