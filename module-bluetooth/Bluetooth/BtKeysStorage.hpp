// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <btstack_link_key_db_memory.h>
#include <btstack_util.h>

#include <json11.hpp>
#include <service-bluetooth/SettingsHolder.hpp>

namespace bluetooth
{

    class KeyStorage
    {
      public:
        static auto getKeyStorage() -> btstack_link_key_db_t *;
        static std::shared_ptr<bluetooth::SettingsHolder> settings;

      private:
        static void openStorage();
        static void closeStorage();
        static auto getLinkKey(uint8_t *bd_addr, link_key_t link_key, link_key_type_t *type) -> int;
        static void putLinkKey(uint8_t *bd_addr, uint8_t *link_key, link_key_type_t type);
        static void deleteLinkKey(uint8_t *bd_addr);
        static auto iterator_init(btstack_link_key_iterator_t *it) -> int;
        static auto iterator_get_next(btstack_link_key_iterator_t *it,
                                      bd_addr_t bd_addr,
                                      link_key_t link_key,
                                      link_key_type_t *type) -> int;
        static void iterator_done(btstack_link_key_iterator_t *it);
        static void set_local_bd_addr(bd_addr_t bd_addr);
        static auto processOnFoundKey(bd_addr_t bd_addr, const std::function<int(json11::Json, bd_addr_t)> &fun) -> int;
        static void writeSettings();
        static json11::Json keysJson;
        static btstack_link_key_db_t keyStorage;
        static json11::Json::array keys;
    };

} // namespace bluetooth
