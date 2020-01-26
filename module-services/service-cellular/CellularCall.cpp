#include "CellularCall.hpp"
#include <Modem/ATParser.hpp>
#include <log/log.hpp>
#include <vector>

namespace ModemCall
{
    ModemCall::ModemCall(const std::string str)
    {
        const std::string prefix = "+CLCC: ";
        std::string callEntry = str;

        // Check for a valid prefix
        if (callEntry.rfind(prefix, 0) != 0)
        {
            LOG_ERROR("no valid prefix");
            LOG_ERROR("callEntry %s", callEntry.c_str());

            throw std::runtime_error("No valid prefix");
        }
        else
        {
            // remove the prefix
            callEntry.erase(0, prefix.length());
        }

        std::vector<std::string> tokens = ATParser::Tokenizer(callEntry, ",");

        auto numberOfTokens = tokens.size();
        if (numberOfTokens != 7 && numberOfTokens != 8)
        {
            LOG_ERROR("wrong number of tokens %u", numberOfTokens);
            throw std::runtime_error("No valid prefix");
        }
        // TODO: alek: add paramters validation
        idx = std::stoul(tokens[0]);
        dir = static_cast<CallDir>(std::stoul(tokens[1]));
        state = static_cast<CallState>(std::stoul(tokens[2]));
        mode = static_cast<CallMode>(std::stoul(tokens[3]));
        isConferenceCall = static_cast<bool>(std::stoul(tokens[4]));
        phoneNumber = tokens[5];
        type = static_cast<CallType>(std::stoul(tokens[6]));
        if (numberOfTokens == 8)
        {
            phoneBookName = tokens[7];
        }
    }

    std::string to_string(const ModemCall &call)
    {
        std::string str = " <idx> " + std::to_string(call.idx) + " <dir> " + std::to_string(static_cast<uint8_t>(call.dir)) + " <stat> " +
                          std::to_string(static_cast<uint8_t>(call.state)) + " <mode> " + std::to_string(static_cast<uint8_t>(call.mode)) + " <mpty> " +
                          std::to_string(static_cast<uint8_t>(call.isConferenceCall)) + " <number> " + call.phoneNumber + " <type> " +
                          std::to_string(static_cast<uint8_t>(call.type));
        if (!call.phoneBookName.empty())
        {
            str += " <alpha> " + call.phoneBookName;
        }

        return str;
    }
} // namespace ModemCall