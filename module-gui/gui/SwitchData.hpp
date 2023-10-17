// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <optional>
#include <log/log.hpp>

namespace gui
{
    using NameOfApplication = std::optional<std::string>;

    /// base class storing information sent along with switch message
    /// for extended use with windows please inherit from this class to extend it
    ///
    /// used combined with:
    /// * Application->switchWindow(...) function changing window within current application
    /// * Application::handleSwitchWindow(...) ApplicationManager call changing window between apps (and within current
    /// if requested)
    class SwitchData
    {
        bool popup = false;

      protected:
        std::string description = "";

      public:
        SwitchData() = default;
        explicit SwitchData(std::string description) : description{std::move(description)} {};

        virtual ~SwitchData() = default;

        [[nodiscard]] virtual const std::string &getDescription() const
        {
            return description;
        };
        virtual void setDescription(const std::string desc)
        {
            description = desc;
        };
        /// informs ApplicationManager to not close caller application when switching from app to app
        bool disableAppClose = false;
        /// informs App window stack that requested window should be ignored on windows stack.
        ///
        /// This affects window back mechanics. Having switchWindow calls like that:
        /// `WinA = { ignoreCurrentWindowOnStack = true } => WinB  => WinC`
        /// requesting getting back in WinC will result in:
        /// `WinC => WinA`
        bool ignoreCurrentWindowOnStack = false;
        /// This can store name of Application when message came from
        /// This is helpful when going back to the previous Application (sender Application)
        NameOfApplication nameOfSenderApplication = std::nullopt;
    };

    class SwitchSpecialChar : public SwitchData
    {
      public:
        std::string requester = "";
        enum class Type
        {
            Request,
            Response,
        } type = Type::Request;
        SwitchSpecialChar(Type type, const std::string requester, const std::string &description = "")
            : SwitchData(description), requester(requester), type(type)
        {}
        virtual ~SwitchSpecialChar() = default;
    };

    /// class storing information for Window type class to switch bask to proper Application
    struct InfoAboutPreviousAppWhereWeComeFrom
    {
        NameOfApplication nameOfPreviousApplication = std::nullopt;
        bool ignoreCurrentApplicationOnSwitchBack   = false;

        // save info about Application where we come from and if we should switch back to this Application
        void saveInfoAboutPreviousAppForProperSwitchBack(const SwitchData *memo)
        {
            if (memo == nullptr) {
                LOG_ERROR("Empty pointer to SwitchData");
                return;
            }
            nameOfPreviousApplication            = memo->nameOfSenderApplication;
            ignoreCurrentApplicationOnSwitchBack = memo->ignoreCurrentWindowOnStack;
        }
        bool shouldCurrentAppBeIgnoredOnSwitchBack()
        {
            return ignoreCurrentApplicationOnSwitchBack && nameOfPreviousApplication.has_value();
        }
    };

} /* namespace gui */
