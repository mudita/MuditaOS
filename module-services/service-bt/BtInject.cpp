#include "BtInject.hpp"
#include <log/log.hpp>
#include <sstream>
#include "service-bt/BtFile.hpp"


static std::string bt_in  = "in:  ";
static std::string bt_out = "out: ";
static std::string bt_file = "bt_commands.txt";


BtInject::BtInject()
{
    file = std::make_unique<BtFile>(bt_file);
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
    while ( line != "" ) 
    {
        line = file->readline();
        std::stringstream ss(line);
        std::string tmp;
        if (line.compare(bt_in) == 0 ) 
        {
            /// td::getline(ss, token, ' ' -> ignore empty
            line = line.erase(0,bt_in.length());
            std::list<char> vals;
            while ( ss >> tmp ) 
            {
                vals.push_back(std::stoi(tmp,0,16));
            }
            LOG_DEBUG("Loaded out command: %s",
                      [&]() -> std::string {
                          std::string str;
                          for (auto el : vals) {
                              str += " ";
                              str += std::to_string(el);
                          }
                          return str;
                      }()
                                   .c_str());
        } else if ( line.compare(bt_out) == 0 ) 
        {
            line = line.erase(0,bt_out.length());
            LOG_DEBUG("TODO");
        } else 
        {
            LOG_ERROR("bad line: %s", line.c_str());
        }
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
