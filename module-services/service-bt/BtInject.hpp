#pragma once

#include <memory>
#include <list>

class BtFile;

class BtInject
{
    protected:
        std::unique_ptr<BtFile> file;
        std::list<std::list<char>> commands;
        std::list<std::list<char>> responses;
    public:
        BtInject();
        std::list<char> popCommand();
        bool checkResponse(char* data, uint32_t len);
        bool parse_commands();
};
