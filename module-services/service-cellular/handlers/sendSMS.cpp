/*
 * sendSMS.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */
#include "../ServiceCellular.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "ucs2/UCS2.hpp"
bool ServiceCellular::sendSMS(void)
{
    auto record = DBServiceAPI::SMSGetLastRecord(this);

    // skip if it's not sms to send
    if (record.type != SMSType::QUEUED)
    {
        return false;
    }

    LOG_INFO("Trying to send SMS");

    uint32_t textLen = record.body.length();

    const uint32_t singleMessageLen = 30;
    bool result = false;

    // if text fit in single message send
    if (textLen < singleMessageLen)
    {

        if (cmux->CheckATCommandPrompt(
                cmux->GetChannel("Commands")
                    ->SendCommandPrompt((std::string(at::factory(at::AT::CMGS)) + UCS2(record.number).modemStr() + "\"\r").c_str(), 1, 1000)))
        {

            if (cmux->GetChannel("Commands")->cmd((UCS2(record.body).modemStr() + "\032").c_str()))
            {
                result = true;
            }
            else
            {
                LOG_INFO("SMS sending failed.");
            }
        }
    }
    // split text, and send concatenated messages
    else
    {
        const uint32_t maxConcatenatedCount = 7;
        uint32_t messagePartsCount = textLen / singleMessageLen;
        if ((textLen % singleMessageLen) != 0)
        {
            messagePartsCount++;
        }

        if (messagePartsCount > maxConcatenatedCount)
        {
            LOG_ERROR("Message to long");
            return false;
        }

        for (uint32_t i = 0; i < messagePartsCount; i++)
        {

            uint32_t partLength = singleMessageLen;
            if (i * singleMessageLen + singleMessageLen > record.body.length())
            {
                partLength = record.body.length() - i * singleMessageLen;
            }
            UTF8 messagePart = record.body.substr(i * singleMessageLen, partLength);

            std::string command(at::factory(at::AT::QCMGS) + UCS2(record.number).modemStr() + "\",120," + std::to_string(i + 1) + "," +
                                std::to_string(messagePartsCount) + "\r");

            if (cmux->CheckATCommandPrompt(cmux->GetChannel("Commands")->SendCommandPrompt(command.c_str(), 1, 5000)))
            {
                // prompt sign received, send data ended by "Ctrl+Z"
                if (cmux->GetChannel("Commands")->cmd((UCS2(messagePart).modemStr() + "\032").c_str(), 2, 2000))
                {
                    result = true;
                }
                else
                {
                    result = false;
                    LOG_INFO("SMS sending failed.");
                }
            }
        }
    }
    if (result)
    {
        LOG_INFO("SMS sended.");
        record.type = SMSType::OUTBOX;
        DBServiceAPI::SMSUpdate(this, record);
    }
    return result;
}
