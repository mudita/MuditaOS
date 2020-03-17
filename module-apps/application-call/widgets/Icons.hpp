#pragma once

#include "Icon.hpp"
#include "i18/i18.hpp"

namespace gui
{
    class AddContactIcon : public Icon
    {
      protected:
        static const inline std::string crossImg      = "cross";
        static const inline std::string addContactStr = "app_call_contact";

      public:
        AddContactIcon() = delete;
        AddContactIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, crossImg, utils::localize.get(addContactStr))
        {}
        ~AddContactIcon() override = default;
    };

    class sendSmsIcon : public Icon
    {
      protected:
        static const inline std::string messageImg  = "mail";
        static const inline std::string sendSmstStr = "app_call_message";

      public:
        sendSmsIcon() = delete;
        sendSmsIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, messageImg, utils::localize.get(sendSmstStr))
        {}
        ~sendSmsIcon() override = default;
    };
} // namespace gui
