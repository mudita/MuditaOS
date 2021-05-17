// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"

class DLCChannel;

namespace nv_paths
{

    auto constexpr voice_domain_pref = "/nv/item_files/modem/mmode/voice_domain_pref";
    auto constexpr sms_domain_pref   = "/nv/item_files/modem/mmode/sms_domain_pref";
    auto constexpr IMS_enable        = "/nv/item_files/ims/IMS_enable";
} // end namespace nv_paths

namespace at
{
    namespace response
    {
        namespace qmbncfg
        {
            enum class MBNAutoSelect
            {
                Off = 0,
                On  = 1
            };

            class MBNConfig
            {
              public:
                std::string fileName;
                std::string version;
                std::string releaseDate;
                unsigned short selected;
                unsigned short activated;
            };
        } // namespace qmbncfg
        bool parseQMBNCFGlist(const at::Result &resp, std::vector<at::response::qmbncfg::MBNConfig> &ret);
    } // namespace response
} // namespace at
/**
 * Manager for Quectel Modem Configuration Binaries command/s
 * MBN files are set of NV/EFS commands.
 *
 *
 * Also encapsulate functionality for Read/Write NV files commands
 * AT+QNVFR=?
 * +QNVFR: <nv_files>
 * AT+QNVFW=?
 * +QNVFW: <nv_files>,<hex_str>,[total_pack],[cur_pack],[write_mode]
 * in basic modes
 */
class QMBNManager
{
  private:
    DLCChannel *channel = nullptr;

  public:
    explicit QMBNManager(DLCChannel *channel) : channel(channel)
    {}

    at::Result::Code list(std::vector<at::response::qmbncfg::MBNConfig> &ret);

    /**
     * Select, will be activated after reboot
     * @return
     */
    at::Result::Code select(const std::string &name);
    /**
     * deactivate and deselect
     * @return
     */
    at::Result::Code deactivate();
    at::Result::Code setAutoSelect(at::response::qmbncfg::MBNAutoSelect value);

    at::Result::Code writeNV(const std::string &nvfile, const std::string &hexvalue);
    at::Result::Code writeNVByte(const std::string &nvfile, std::uint8_t byte);
    at::Result::Code readNV(const std::string &nvfile, std::string &hexret);
    at::Result::Code readNVByte(const std::string &nvfile, std::uint8_t &byte);
};
