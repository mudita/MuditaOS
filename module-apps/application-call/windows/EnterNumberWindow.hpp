/*
 * @file EnterNumberWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "../ApplicationCall.hpp"
#include "../widgets/Icons.hpp"

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
        gui::Label *numberLabel             = nullptr;
        gui::AddContactIcon *newContactIcon = nullptr;

        void setNumberLabel(const std::string num);

      public:
        EnterNumberWindow() = delete;
        EnterNumberWindow(app::Application *app, std::string windowName = app::window::name_enterNumber);
        virtual ~EnterNumberWindow();

        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;
        const std::string &getEnteredNumber() const noexcept;

        void rebuild() override;
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

        bool addNewContact();
        void switchFormatter(const std::string &countryCode);
        void initFormatterInput(const std::string &number);
        void addDigit(const std::string::value_type &digit);
        void clearInput();
    };

} /* namespace gui */
