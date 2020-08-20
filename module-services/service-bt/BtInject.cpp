#include "BtInject.hpp"
#include <log/log.hpp>
#include <sstream>
#include "service-bt/BtFile.hpp"


static std::string bt_in  = "in:  ";
static std::string bt_out = "out: ";
static std::string bt_file = "bt_commands.txt";


BtInject::BtInject()
{
    file = std::make_unique<BtFile>(bt_file, "r");
}

bool BtInject::parse_line(const std::string &type, std::string &line, Commands &list) const
{
    if (line.length() > type.length() && line.compare(0, type.length(), type) == 0) {
        line.erase(0, type.length());
        std::stringstream ss(line);
        std::list<char> vals;
        std::string tmp;
        while (ss >> tmp) {
            if (tmp.length() == 0) {
                continue;
            }
            vals.push_back(std::stoul(tmp, nullptr, 16));
        }
        // LOG_DEBUG("Loaded command: %s",
        //           [&]() -> std::string {
        //               std::stringstream ss;
        //               for (auto el : vals) {
        //                   ss << " 0x" << std::hex << (int)el;
        //               }
        //               return ss.str();
        //           }()
        //                        .c_str());
        list.push_back(vals);
        return true;
    }
    return false;
}

/// Format: command - space delimited hexes
bool BtInject::parse_commands()
{
    if (file == nullptr ) {
        LOG_ERROR("cant parse - no %s file", bt_file.c_str());
        return false;
    }
    std::string line = file->readline();
    if( line == "") 
    {
        LOG_ERROR("empty file!");
    }
    while (line != "") {
        line = file->readline();
        if (parse_line(bt_in, line, commands)) {
            continue;
        }

        if (parse_line(bt_out, line, responses)) {
            continue;
        }
        LOG_ERROR("[in: %d out: %d] bad line: %s", line.compare(bt_in), line.compare(bt_out), line.c_str());
    }
    LOG_DEBUG("File parsing success!");
    return true;
}

static auto pop(std::list<std::list<char>> &list)  -> std::list<char>
{
    if( list.size() == 0 ) 
    {
        auto ret = list.front();
        list.pop_front();
        return ret;
    }
    return {};
}

std::list<char> BtInject::popCommand() 
{
    return pop(commands);
}

bool BtInject::checkResponse(char *data, uint32_t len)
{
    std::list<char> to_check = pop(responses);
    std::list<char> result = std::list<char>(data, data+len);
    return to_check == result;
}
