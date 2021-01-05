// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <purefs/filesystem_paths.hpp>
#include <gsl/gsl_util>

#include "BtKeysStorage.hpp"
#include <log/log.hpp>

json11::Json Bt::KeyStorage::fileJson = json11::Json();
btstack_link_key_db_t Bt::KeyStorage::keyStorage;
json11::Json::array Bt::KeyStorage::keys;
std::string Bt::KeyStorage::fileContent;

namespace Bt
{
    namespace
    {
        std::string loadFilesAsString(const std::filesystem::path &fileToLoad)
        {
            using namespace std::string_literals;
            static constexpr auto file_size_limit = 512LU * 1024LU;
            std::error_code ec;
            auto filesize = std::filesystem::file_size(fileToLoad, ec);
            if (ec || filesize > file_size_limit) {
                return ""s;
            }
            std::string contents(filesize, '\0');
            auto fp = fopen(fileToLoad.c_str(), "r");
            if (!fp) {
                return ""s;
            }
            auto cleanup      = gsl::finally([fp] { fclose(fp); });
            const auto nitems = std::fread(contents.data(), contents.size(), 1, fp);
            return (nitems == 1) ? contents : ""s;
        }

        bool replaceWithString(const std::filesystem::path &fileToModify, const std::string &stringToWrite)
        {
            auto fp = std::fopen(fileToModify.c_str(), "w");
            if (!fp)
                return false;
            auto cleanup       = gsl::finally([fp] { fclose(fp); });
            size_t dataWritten = std::fwrite(stringToWrite.data(), stringToWrite.size(), 1, fp);
            return dataWritten == 1;
        }
    } // namespace

    namespace strings
    {
        inline std::string keysFilename = purefs::dir::getUserDiskPath() / "btkeys.json";
        inline std::string keys         = "keys";
        inline std::string link_key     = "link_key";
        inline std::string bd_addr      = "bd_addr";
        inline std::string type         = "type";
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
        fileContent.clear();
        fileContent = loadFilesAsString(strings::keysFilename);
        if (fileContent.empty()) {
            LOG_WARN("opening empty key file!");
            return;
        }

        std::string err;
        fileJson = json11::Json::parse(fileContent.c_str(), err);
        if (!err.empty()) {
            LOG_ERROR("Error while parsing json: %s", err.c_str());
            return;
        }

        keys = std::move(fileJson[strings::keys].array_items());
        LOG_INFO("Imported keys: %d", static_cast<unsigned int>(keys.size()));
    }

    void KeyStorage::closeStorage()
    {
        LOG_INFO("closing storage from API");
        writeToFile();
    }
    //
    auto KeyStorage::getLinkKey(uint8_t *bd_addr, link_key_t link_key, link_key_type_t *type) -> int
    {
        if (type != nullptr && bd_addr != nullptr) {
            LOG_INFO("getting key for address %s from API", bd_addr_to_str(bd_addr));
            if (keys.size() == 0) {
                return 0;
            }
            for (auto key : keys) {
                bd_addr_t addr;
                sscanf_bd_addr(key[strings::bd_addr].string_value().c_str(), addr);

                if (bd_addr_cmp(addr, bd_addr) == 0) {
                    auto foundLinkKey = key[strings::link_key].string_value().c_str();
                    memcpy(link_key, foundLinkKey, sizeof(link_key_t));
                    *type = static_cast<link_key_type_t>(key[strings::type].int_value());

                    return 1;
                }
            }
        }
        return 0;
    }
    void KeyStorage::putLinkKey(uint8_t *bd_addr, uint8_t *link_key, link_key_type_t type)
    {
        LOG_INFO("putting key for address %s from API", bd_addr_to_str(bd_addr));
        auto keyEntry = json11::Json::object{{strings::bd_addr, bd_addr_to_str(bd_addr)},
                                             {strings::link_key, std::string(reinterpret_cast<char *>(link_key))},
                                             {strings::type, type}};
        keys.emplace_back(keyEntry);
        writeToFile();
        LOG_INFO("keys written to the file");
        LOG_INFO("Keys in file: %d", (int)keys.size());
    }
    void KeyStorage::deleteLinkKey(uint8_t *bd_addr)
    {
        auto keysSize = keys.size();
        LOG_INFO("deleting key for address %s from API", bd_addr_to_str(bd_addr));
        auto end = std::remove_if(keys.begin(), keys.end(), [&](auto &key) {
            bd_addr_t addr;
            sscanf_bd_addr(key[strings::bd_addr].string_value().c_str(), addr);
            return bd_addr_cmp(addr, bd_addr) == 0;
        });

        keys.erase(end, keys.end());
        if (keysSize != keys.size()) {
            LOG_INFO("Key successfully deleted");
        }
        writeToFile();
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
    void KeyStorage::writeToFile()
    {
        json11::Json finalJson = json11::Json::object{{strings::keys, keys}};
        fileContent            = finalJson.dump();
        replaceWithString(strings::keysFilename, fileContent);
    }

} // namespace Bt
