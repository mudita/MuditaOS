#pragma once

#include "Icon.hpp"

namespace gui
{
    // Icon widget with custom image and label
    class AddContactIcon : public Icon
    {
      protected:
        static const inline std::string crossImg      = "cross";
        static const inline std::string addContactStr = "app_call_contact";

      public:
        AddContactIcon() = delete;
        AddContactIcon(Item *parent, const uint32_t &x, const uint32_t &y);
        ~AddContactIcon() override = default;
    };
} // namespace gui