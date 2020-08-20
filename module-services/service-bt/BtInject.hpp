#pragma once

#include <memory>
#include <list>

class BtFile;

class BtInject
{
    using Command = std::list<char>;
    using Commands = std::list<Command>;
    protected:
        std::unique_ptr<BtFile> file;
        Commands commands;
        std::list<std::list<char>> responses;
    public:
        BtInject();
        std::list<char> popCommand();
        bool checkResponse(char* data, uint32_t len);
        bool parse_commands();
        bool parse_line(const std::string &type, std::string &line, Commands &list) const;
        const Commands & getCommands() { return commands; };
};
