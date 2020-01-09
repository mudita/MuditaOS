#ifndef __packet_HPP
#define __packet_HPP

#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

#define PACKET_HEADER 0x43520000
#define PACKET_MAX_LENGTH 1024

/*
    Packet header byte 1 & 0 are unique message ID. This id should have the same value if message is a return value for specific command. In multipart message
   all parth should have the same ID. If multipart message is sent then length byte 3 & 2 are message part number in descending order - eg. last part should be
   '0'
*/

class packet
{
  public:
    packet(){

    };
    ~packet(){

    };

    enum class packet_type_t
    {
        pt_none,
        pt_test,
        pt_command,
        pt_data,
        pt_multipart,
    };

    typedef struct packet_s
    {
        uint32_t header;     // 4
        uint32_t length;     // 4
        packet_type_t type;  // 4
        uint8_t test_number; // 1

        std::vector<uint8_t> serialize()
        {
            std::vector<uint8_t> out;
            out.push_back(static_cast<uint8_t>((header & 0xFF000000) >> 24));
            out.push_back(static_cast<uint8_t>((header & 0x00FF0000) >> 16));
            out.push_back(static_cast<uint8_t>((header & 0x0000FF00) >> 8));
            out.push_back(static_cast<uint8_t>(header & 0x000000FF));
            out.push_back(static_cast<uint8_t>((length & 0xFF000000) >> 24));
            out.push_back(static_cast<uint8_t>((length & 0x00FF0000) >> 16));
            out.push_back(static_cast<uint8_t>((length & 0x0000FF00) >> 8));
            out.push_back(static_cast<uint8_t>(length & 0x000000FF));
            out.push_back(static_cast<uint8_t>((static_cast<uint32_t>(type) & 0xFF000000) >> 24));
            out.push_back(static_cast<uint8_t>((static_cast<uint32_t>(type) & 0x00FF0000) >> 16));
            out.push_back(static_cast<uint8_t>((static_cast<uint32_t>(type) & 0x0000FF00) >> 8));
            out.push_back(static_cast<uint8_t>(static_cast<uint32_t>(type) & 0x000000FF));
            out.push_back(test_number);
            return out;
        };

        void deserialize(std::vector<uint8_t> in)
        {
            uint32_t tmp = 0;
            tmp |= static_cast<uint32_t>(in[0]) << 24;
            tmp |= static_cast<uint32_t>(in[1]) << 16;
            tmp |= static_cast<uint32_t>(in[2]) << 8;
            tmp |= static_cast<uint32_t>(in[3]);
            header = tmp;
            tmp = 0;
            tmp |= static_cast<uint32_t>(in[4]) << 24;
            tmp |= static_cast<uint32_t>(in[5]) << 16;
            tmp |= static_cast<uint32_t>(in[6]) << 8;
            tmp |= static_cast<uint32_t>(in[7]);
            length = tmp;
            tmp = 0;
            tmp |= static_cast<uint32_t>(in[8]) << 24;
            tmp |= static_cast<uint32_t>(in[9]) << 16;
            tmp |= static_cast<uint32_t>(in[10]) << 8;
            tmp |= static_cast<uint32_t>(in[11]);
            type = static_cast<packet_type_t>(tmp);
            test_number = in[12];
        };
    } packet_t;

  private:
    std::vector<uint8_t> _payload;
    packet_t _header;

  public:
    void set_payload(const std::vector<uint8_t> &payload)
    {
        _payload = payload;
    }
    const std::vector<uint8_t> &get_payload()
    {
        return _payload;
    }
    void set_header(const packet_t &header)
    {
        _header = header;
    }
    const packet_t &get_header()
    {
        return _header;
    }
    const std::vector<uint8_t> serialize()
    {
        std::vector<uint8_t> out;

        for (const auto &i : _header.serialize())
        {
            out.push_back(i);
        }

        for (const auto &i : _payload)
        {
            out.push_back(i);
        }

        return out;
    }

    void deserialize(std::vector<uint8_t> in)
    {
        _header.deserialize(in);
        std::vector<uint8_t> v(in.begin() + 13, in.end());
        _payload = v;
    }

    void deserialize(uint8_t in[], uint32_t len)
    {
        std::vector<uint8_t> v;
        for (uint32_t i = 0; i < len; i++)
            v.push_back(in[i]);
        deserialize(v);
    }
};

#endif