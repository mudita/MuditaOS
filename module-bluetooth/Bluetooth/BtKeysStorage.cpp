// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <algorithm>
#include "BtKeysStorage.hpp"
#include <log/log.hpp>

json11::Json bluetooth::KeyStorage::keysJson = json11::Json();
btstack_link_key_db_t bluetooth::KeyStorage::keyStorage;
json11::Json::array bluetooth::KeyStorage::keys;
std::shared_ptr<bluetooth::SettingsHolder> bluetooth::KeyStorage::settings = nullptr;

namespace bluetooth
{
    namespace strings
    {
        constexpr inline auto keys     = "keys";
        constexpr inline auto link_key = "link_key";
        constexpr inline auto bd_addr  = "bd_addr";
        constexpr inline auto type     = "type";
    } // namespace strings

    auto KeyStorage::getKeyStorage() -> btstack_link_key_db_t *
    {
        keyStorage.open              = openStorage;
        keyStorage.set_local_bd_addr = set_local_bd_addr;
        keyStorage.close             = closeStorage;
        keyStorage.get_link_key      = getLinkKey;
        keyStorage.put_link_key      = putLinkKey;
        keyStorage.delete_link_key   = deleteLinkKey;
        keyStorage.iterator_init     = iterator_init;
        keyStorage.iterator_get_next = iterator_get_next;
        keyStorage.iterator_done     = iterator_done;

        return &keyStorage;
    }

    void KeyStorage::openStorage()
    {
        LOG_INFO("opening storage from API");
        std::string keysEntry;
        if (settings) {
            keysEntry = std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::BtKeys));
        }
        else {
            LOG_ERROR("failed opening settings for BT!");
            return;
        }
        if (keysEntry.empty()) {
            LOG_WARN("opening empty key entry!");
            return;
        }

        std::string err;
        keysJson = json11::Json::parse(keysEntry.c_str(), err);
        if (!err.empty()) {
            LOG_ERROR("Error while parsing json: %s", err.c_str());
            return;
        }

        keys = std::move(keysJson[strings::keys].array_items());
    }

    void KeyStorage::closeStorage()
    {
        LOG_INFO("closing storage from API");
        writeSettings();
    }
    //
    auto KeyStorage::getLinkKey(uint8_t *bd_addr, link_key_t link_key, link_key_type_t *type) -> int
    {
        if (type != nullptr && bd_addr != nullptr) {
            LOG_INFO("getting key from API");

            json11::Json finalJson = json11::Json::object{{strings::keys, keys}};
            auto keysEntryDump     = finalJson.dump();
            LOG_INFO("Current keys state: %s", keysEntryDump.c_str());

            if (keys.empty()) {
                LOG_ERROR("Keys empty!");
                return 0;
            }

            auto ret = processOnFoundKey(bd_addr, [&type, &link_key](const json11::Json &key, bd_addr_t) {
                auto foundLinkKey = key[strings::link_key].string_value().c_str();
                memcpy(link_key, reinterpret_cast<const uint8_t *>(foundLinkKey), sizeof(link_key_t));
                LOG_INFO("Getting key: %s", foundLinkKey);
                *type = static_cast<link_key_type_t>(key[strings::type].int_value());

                return 1;
            });
            if (ret == 0) {
                LOG_ERROR("Can't find key for this address!");
            }
            return ret;
        }
        return 0;
    }
    void KeyStorage::putLinkKey(uint8_t *bd_addr, uint8_t *link_key, link_key_type_t type)
    {

        processOnFoundKey(bd_addr, [](const json11::Json &, bd_addr_t bd_addr) {
            LOG_ERROR("Key already found in the key storage - deleting!");
            deleteLinkKey(bd_addr);
            return 1;
        });

        auto keyEntry = json11::Json::object{{strings::bd_addr, bd_addr_to_str(bd_addr)},
                                             {strings::link_key, std::string(reinterpret_cast<char *>(link_key))},
                                             {strings::type, type}};
        keys.emplace_back(keyEntry);

        if (settings->onLinkKeyAdded) {
            settings->onLinkKeyAdded(bd_addr_to_str(bd_addr));
        }
        writeSettings();
        LOG_INFO("keys written to the file");
        LOG_INFO("Keys in file: %d", (int)keys.size());
    }
    void KeyStorage::deleteLinkKey(uint8_t *bd_addr)
    {
        auto keysSize = keys.size();
        LOG_INFO("deleting key from API");
        auto end = std::remove_if(keys.begin(), keys.end(), [&](auto &key) {
            bd_addr_t addr;
            sscanf_bd_addr(key[strings::bd_addr].string_value().c_str(), addr);
            return bd_addr_cmp(addr, bd_addr) == 0;
        });

        keys.erase(end, keys.end());
        if (keysSize != keys.size()) {
            LOG_INFO("Key successfully deleted");
        }
        writeSettings();
    }
    //
    auto KeyStorage::iterator_init(btstack_link_key_iterator_t *it) -> int
    {
        return 0;
    }
    auto KeyStorage::iterator_get_next(btstack_link_key_iterator_t *it,
                                       uint8_t *bd_addr,
                                       uint8_t *link_key,
                                       link_key_type_t *type) -> int
    {
        return 0;
    }
    void KeyStorage::iterator_done(btstack_link_key_iterator_t *it)
    {}
    void KeyStorage::set_local_bd_addr(bd_addr_t bd_addr)
    {}
    void KeyStorage::writeSettings()
    {
        json11::Json finalJson = json11::Json::object{{strings::keys, keys}};
        auto keysEntry         = finalJson.dump();
        if (settings) {
            settings->setValue(bluetooth::Settings::BtKeys, keysEntry);
        }
        else {
            LOG_ERROR("failed to open settings to write!");
            return;
        }
    }
    auto KeyStorage::processOnFoundKey(bd_addr_t bd_addr, const std::function<int(json11::Json, bd_addr_t)> &fun) -> int
    {
        for (const auto &key : keys) {
            bd_addr_t addr;
            sscanf_bd_addr(key[strings::bd_addr].string_value().c_str(), addr);

            if (bd_addr_cmp(addr, bd_addr) == 0 && fun) {
                return fun(key, bd_addr);
            }
        }
        return 0;
    }

} // namespace bluetooth
