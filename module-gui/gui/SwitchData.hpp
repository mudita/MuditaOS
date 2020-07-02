#pragma once

#include <string>

namespace gui
{

    /// base class storing information sent along with switch message
    /// for extended use with windows please inherit from this class to extend it
    ///
    /// used combined with:
    /// * Application->switchWindow(...) function changing window within current application
    /// * Application::handleSwitchWindow(...) ApplicationManager call changing window between apps (and within current
    /// if requested)
    class SwitchData
    {
      protected:
        std::string description = "";

      public:
        SwitchData() = default;
        SwitchData(const std::string &description) : description{description} {};
        virtual ~SwitchData() = default;
        virtual const std::string &getDescription() const
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

} /* namespace gui */
