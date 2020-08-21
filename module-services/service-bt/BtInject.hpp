#pragma once

#include <memory>
#include <list>
#include "BtFile.hpp"

class BtInject
{
    public:
        using Command = std::list<char>;
        using Commands = std::list<Command>;
    protected:
        std::unique_ptr<BtFile> file;
        Commands commands;
        Commands responses;
    public:
        BtInject();
        std::list<char> popCommand();
        bool parse_commands();
        bool parse_line(const std::string &type, std::string &line, Commands &list) const;
        Commands & getCommands() { return commands; };
        const Commands & getResponses() { return responses; };
};
