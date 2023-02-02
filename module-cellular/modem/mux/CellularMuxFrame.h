// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#pragma GCC push_options
#pragma GCC optimize("O0,no-omit-frame-pointer")

#include "CellularMuxTypes.h"
#include <inttypes.h>
#include <vector>
#include <iostream>
#include <log/log.hpp>
#include <cstdio>

#define TS0710_FLAG          0xF9
#define TS0710_FRAME_HDR_LEN 6 // without extended address byte

class CellularMuxFrame
{
  private:
    static constexpr unsigned char crctable[256] = { // reversed, 8-bit, poly=0x07
        0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B, 0x1C, 0x8D,
        0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67, 0x38, 0xA9, 0xDB, 0x4A,
        0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43, 0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2,
        0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F, 0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
        0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B, 0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3,
        0x81, 0x10, 0x65, 0xF4, 0x86, 0x17, 0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34,
        0x41, 0xD0, 0xA2, 0x33, 0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC,
        0xBE, 0x2F, 0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
        0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87, 0xD8, 0x49,
        0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3, 0xC4, 0x55, 0x27, 0xB6,
        0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF, 0x90, 0x01, 0x73, 0xE2, 0x97, 0x06,
        0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB, 0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
        0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7, 0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37,
        0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3, 0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8,
        0xBD, 0x2C, 0x5E, 0xCF};

  public:
    enum TS0710FrameStatus : std::uint8_t
    {
        OK,
        EmptyFrame,
        IncorrectStartStopFlags,
        CRCError,
    };

    struct frame_t
    {
        TS0710FrameStatus frameStatus = OK;
        uint8_t Address;
        uint8_t Control;
        std::vector<uint8_t> data;

        frame_t() = default;

        explicit frame_t(uint8_t address, uint8_t control) : frameStatus{OK}, Address{address}, Control{control}
        {}

        std::vector<uint8_t> serialize()
        {
            std::vector<uint8_t> ret;
            ret.push_back(TS0710_FLAG);
            ret.push_back(Address | 0x01); // add EA = 1
            ret.push_back(Control);
            uint16_t Length = static_cast<uint16_t>(data.size());
            if (Length > 127) {
                Length <<= 1;      // shift data to extend for E/A bit
                Length &= ~0x0001; // add E/A bit = 0 indicating long length
                ret.push_back(static_cast<uint8_t>(Length & 0x00FF));
                ret.push_back(static_cast<uint8_t>((Length >> 8) & 0x00FF));
            }
            else {
                Length <<= 1;     // shift data to extend for E/A bit
                Length |= 0x0001; // add E/A bit = 1 indicating short length
                ret.push_back(static_cast<uint8_t>(Length & 0x00FF));
            }
            ret.insert(ret.end(), data.begin(), data.end()); // add data
            /*Calculate FCS*/
            unsigned char FCS = 0xFF;
            unsigned char len = ret.size() - 1;
            unsigned char i   = 1;
            if (Control == static_cast<uint8_t>(TypeOfFrame_e::UIH)) {
                len = 3; // par. 5.3.6 of GSM0710 document states that for UIH frames, the FCS shall be calculated over
                // only the address, control and length fields TODO: include 2-byte address
                // if (Length > 127)
                //     len += 1;
            }
            /*len is the number of bytes in the message, p points to message*/

            while (len--) {
                FCS = crctable[FCS ^ ret[i++]];
            }
            /*Ones complement*/
            FCS = 0xFF - FCS;
            ret.push_back(FCS);
            ret.push_back(TS0710_FLAG);
            return ret;
        }

        void logFrame(std::vector<uint8_t> const& input)
        {
            std::string logStr("\n*********** Frame content:\n\t\t");
            char buf[3];
            bool wasHex = false;

            for (auto bb: input) {
                memset(buf, 0, sizeof(buf));
                if (std::isprint(bb)) {
                    if (wasHex) {
                        logStr += " ";
                        wasHex = false;
                    }
                    sprintf(buf, "%c", bb);
                    logStr += buf;
                } else {
                    sprintf(buf, "%02X", bb);
                    logStr += " 0x" + std::string(buf);
                    wasHex = true;
                }
            }

            LOG_WARN("%s", logStr.c_str());
        }

        volatile int dbgBreakCnt_;
        void deserialize(const std::vector<uint8_t> &serData)
        {
            std::string serDataStr;
            for (auto bb: serData) {
                serDataStr.append(1, static_cast<char>(bb));
            }
            if (serDataStr.find("servingcell") != serDataStr.npos) {
                ++dbgBreakCnt_;
            }

            logFrame(serData);

            if (serData.size() < 4) {
                LOG_ERROR("Trying to deserialize empty frame");
                Address     = 0;
                Control     = 0;
                frameStatus = EmptyFrame;
                return;
            }
            // iterate through frame to get correct trailing flag. In case multiple-frame stream provided
            bool myFrame  = false;
            ssize_t myLen = 0;
            for (uint8_t c : serData) {
                myLen++;
                if ((!myFrame) && (c == TS0710_FLAG)) {
                    myFrame = true;
                    continue;
                }
                if ((myFrame) && (c == TS0710_FLAG)) {
                    break;
                }
            }

            if ((serData[0] != TS0710_FLAG) || (serData[myLen - 1] != TS0710_FLAG)) {
                Address     = 0;
                Control     = 0;
                frameStatus = IncorrectStartStopFlags;
                LOG_ERROR("Received frame has incorrect leading/trailing flags. Dropping.");
                return; // return empty frame. Discard frame witout proper leading & trailing flag
            }

            Address = serData[1];
            Control = serData[2] & ~(1 << 4);
            int Length;
            if (serData[3] & 0x01) { // short length
                Length = static_cast<uint16_t>(serData[3] >> 1);
            }
            else { // long length
                Length = static_cast<uint16_t>(serData[3] >> 1) + (static_cast<uint16_t>(serData[3]) << 7);
            }
            if (serData[3] == 0xFF) { // ugly hack for Quectel misimplementation of the standard
                if (myLen < 6) {
                    LOG_ERROR("The size of the hacked frame is less than 6 bytes. Dropping...");
                    Address     = 0;
                    Control     = 0;
                    frameStatus = EmptyFrame;
                    return;
                }
                else
                    Length = myLen - 6;
            }

            data.clear();
            data.insert(data.begin(),
                        serData.begin() + 4,
                        serData.begin() + 4 + Length); // get data - amount of data determined by Length value

            // check FCS
            /*Calculate FCS*/
            unsigned char FCS = 0xFF;
            unsigned char len = myLen - 2;
            unsigned char i   = 1;
            if (Control == static_cast<uint8_t>(TypeOfFrame_e::UIH)) {
                len = 3; // par. 5.3.6 of GSM0710 document states that for UIH frames, the FCS shall be calculated over
                // only the address, control and length fields
                // if (Length > 127)
                //     len += 1;
            }
            /*len is the number of bytes in the message, p points to message*/
            while (len--) {
                FCS = crctable[FCS ^ serData[i++]];
            }
            if (Control == static_cast<uint8_t>(TypeOfFrame_e::UIH))
                FCS = crctable[FCS ^ serData[myLen - 2]];

            if ((FCS != 0xCF) && (serData[3] != 0xFF) &&
                (Control !=
                 static_cast<uint8_t>(
                     TypeOfFrame_e::UA))) { // error - but ignore FCS check if it's faulty Quectel UIH frame or UA frame
                LOG_PRINTF("\n{");
                for ([[maybe_unused]] const auto &el : serData) {
                    LOG_SENSITIVE(LOGDEBUG, "%02X ", el);
                }
                LOG_PRINTF("}\n");

                Address     = 0;
                Control     = 0;
                frameStatus = CRCError;
                data.clear();
                LOG_ERROR("Received frame FCS [0x%02X] != 0xCF error. Dropping.", FCS);
                return; // return empty frame. Discard frame witout proper leading & trailing flag
            }
        }
    };

  private:
    frame_t pv_frame;
    std::vector<uint8_t> pv_serData;

  public:
    explicit CellularMuxFrame(frame_t frame)
    {
        pv_serData = frame.serialize();
        pv_frame   = frame;
    }

    explicit CellularMuxFrame(const std::vector<uint8_t> &serData)
    {
        pv_frame.deserialize(serData);
        pv_serData = serData;
    }

    CellularMuxFrame()
    {
        LOG_DEBUG("Deserializing pv_serData");
        pv_frame.deserialize(pv_serData);
    }

    ~CellularMuxFrame()
    {
        pv_serData.clear();
        pv_frame.data.clear();
    }

    frame_t &getFrame()
    {
        return pv_frame;
    }

    const std::vector<uint8_t> &getData() const noexcept
    {
        return pv_frame.data;
    }

    std::vector<uint8_t> getSerData()
    {
        return pv_serData;
    }

    /* F9 03 3F 01 1C F9 */
    static bool isComplete(const std::vector<uint8_t> &serData)
    {
        if (serData.size() < 4) {
            return false; // check if buffer has enough data to get length
        }

        if ((serData[0] != TS0710_FLAG) || (serData[serData.size() - 1] != TS0710_FLAG)) {
            return false;
        }

        int Length = 0;
        if (serData[3] & 0x01) { // short length
            Length = static_cast<uint16_t>(serData[3] >> 1);
        }
        else if (serData.size() > 4) { // long length - another check if enough bytes in buffer
            Length = static_cast<uint16_t>(serData[3] >> 1) + (static_cast<uint16_t>(serData[4]) << 7);
        }
        else {
            return false;
        }

        if (serData.size() >=
            static_cast<size_t>(TS0710_FRAME_HDR_LEN + Length +
                                (serData[3] & 0x01 ? 0 : 1))) { // include extended address byte if present
            return true;
        }

        return false;
    }

    bool isMyChannel(DLCI_t DLCI) const
    {
        return (pv_serData.size() > 1) && ((pv_serData[1] >> 2) == DLCI);
    }

    static DLCI_t getFrameDLCI(const std::vector<uint8_t> &serData)
    {
        if (serData.size() > 1)
            return (serData[1] >> 2);
        return -1;
    }

    DLCI_t getFrameDLCI() const
    {
        if (pv_serData.size() > 1)
            return (pv_serData[1] >> 2);
        return -1;
    }

    TS0710FrameStatus getFrameStatus() const
    {
        return pv_frame.frameStatus;
    }
};

#pragma GCC pop_options
