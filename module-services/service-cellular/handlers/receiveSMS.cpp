/*
 * receiveSMS.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */
#include <sstream>

#include "../ServiceCellular.hpp"
#include "service-db/api/DBServiceAPI.hpp"

#include "time/time_conversion.hpp"
#include "ucs2/UCS2.hpp"
#include <Utils.hpp>

bool ServiceCellular::receiveSMS(std::string messageNumber)
{

    auto cmd = at::factory(at::AT::QCMGR);
    auto ret = cmux->GetChannel("Commands")->cmd(cmd + messageNumber + "\r", cmd.timeout);

    bool messageParsed = false;

    std::string messageRawBody;
    UTF8 receivedNumber;
    if (ret)
    {
        for (uint32_t i = 0; i < ret.response.size(); i++)
        {
            if (ret.response[i].find("QCMGR") != std::string::npos)
            {

                std::istringstream ss(ret.response[i]);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, ','))
                {
                    tokens.push_back(token);
                }
                tokens[1].erase(std::remove(tokens[1].begin(), tokens[1].end(), '\"'), tokens[1].end());
                /*
                 * tokens:
                 * [0] - +QCMGR
                 * [1] - sender number
                 * [2] - none
                 * [3] - date YY/MM/DD
                 * [4] - hour HH/MM/SS/timezone
                 * concatenaded messages
                 * [5] - unique concatenated message id
                 * [6] - current message number
                 * [7] - total messages count
                 *
                 */
                // parse sender number
                receivedNumber = UCS2(tokens[1]).toUTF8();

                // parse date
                tokens[3].erase(std::remove(tokens[3].begin(), tokens[3].end(), '\"'), tokens[3].end());

                utils::time::Timestamp time;
                time.set_time(tokens[3] + " " + tokens[4], "%y/%m/%d %H:%M:%S");
                auto messageDate = time.getTime();

                // if its single message process
                if (tokens.size() == 5)
                {

                    messageRawBody = ret.response[i + 1];
                    messageParsed = true;
                }
                // if its concatenated message wait for last message
                else if (tokens.size() == 8)
                {

                    uint32_t last = 0;
                    uint32_t current = 0;
                    try
                    {
                        last = std::stoi(tokens[7]);
                        current = std::stoi(tokens[6]);
                    }
                    catch (const std::exception &e)
                    {
                        LOG_ERROR("ServiceCellular::receiveSMS error %s", e.what());
                        return false;
                    }
                    if (current == last)
                    {
                        messageParts.push_back(ret.response[i + 1]);

                        for (uint32_t j = 0; j < messageParts.size(); j++)
                        {
                            messageRawBody += messageParts[j];
                        }
                        messageParts.clear();
                        messageParsed = true;
                    }
                    else
                    {
                        messageParts.push_back(ret.response[i + 1]);
                    }
                }
                if (messageParsed)
                {
                    messageParsed = false;

                    UTF8 decodedMessage = UCS2(messageRawBody).toUTF8();

                    SMSRecord record;
                    record.body = decodedMessage;
                    record.number = receivedNumber;
                    record.type = SMSType::INBOX;
                    record.isRead = true;
                    record.date = messageDate;

                    DBServiceAPI::SMSAdd(this, record);
                }
            }
        }
    }
    // delete message from modem memory
    cmux->GetChannel("Commands")->cmd(at::factory(at::AT::CMGD) + messageNumber);
    return true;
}
