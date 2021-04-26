// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsMessages.hpp"
#include <Service/ServiceInterface.hpp>
#include <module-sys/Service/MessageForward.hpp>
#include <functional>

namespace sys
{
    using MessageHandler = std::function<MessagePointer(Message *)>;
}

namespace settings
{
    class SettingsCache;

    class Interface : protected service::Interface
    {
      private:
        std::string dbAgentName = service::name::db;

      public:
        Interface() = default;
        explicit Interface(const service::Interface &interface);
        std::string ownerName();
        void subscribeDB();
        SettingsCache *getCache();
        const std::string &agentName()
        {
            return dbAgentName;
        }

        enum class Change
        {
            Register,
            Deregister
        };

        void changeHandler(const std::type_info &what, Change change, sys::MessageHandler foo);
        void sendMsg(std::shared_ptr<settings::Messages::SettingsMessage> &&msg);
        explicit operator bool() noexcept;
    };
} // namespace settings
