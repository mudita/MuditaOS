// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <SMSTemplateRecord.hpp>
#include <ContactRecord.hpp>
#include <ThreadRecord.hpp>
#include <SMSRecord.hpp>
#include <Database/Database.hpp>
#include <memory>
#include <string>
#include <utility>

class SMSThreadData : public gui::SwitchData
{
  public:
    std::shared_ptr<ThreadRecord> thread;
    std::optional<UTF8> threadName;

    SMSThreadData(std::shared_ptr<ThreadRecord> _thread, std::optional<UTF8> _threadName = std::nullopt)
        : thread(std::move(_thread)), threadName{std::move(_threadName)}
    {}
};

class SMSSwitchData : public gui::SwitchData
{
  protected:
    SMSRecord record;

  public:
    SMSSwitchData() = delete;
    explicit SMSSwitchData(SMSRecord record) : record(std::move(record))
    {}
    ~SMSSwitchData() override = default;

    [[nodiscard]] auto getRecord() const noexcept -> const SMSRecord &
    {
        return record;
    };
};

class SMSRequest : public gui::SwitchData
{
  protected:
    utils::PhoneNumber::View phoneNumber;

  public:
    SMSRequest() = delete;
    SMSRequest(const utils::PhoneNumber::View &phoneNumber) : phoneNumber(phoneNumber)
    {}
    ~SMSRequest() override = default;

    [[nodiscard]] auto getPhoneNumber() const -> const utils::PhoneNumber::View &
    {
        return phoneNumber;
    };
};

class SMSSendRequest : public SMSRequest
{
  public:
    SMSSendRequest(const utils::PhoneNumber::View &phoneNumber, UTF8 textData)
        : SMSRequest(phoneNumber), textData(std::move(textData))
    {}
    ~SMSSendRequest() override = default;
    UTF8 textData;
};

class SMSSendTemplateRequest : public SMSRequest
{
  public:
    SMSSendTemplateRequest(const utils::PhoneNumber::View &phoneNumber, bool preventAutoLock = false)
        : SMSRequest(phoneNumber), preventAutoLock(preventAutoLock)
    {}
    ~SMSSendTemplateRequest() override = default;

    [[nodiscard]] bool isAutoLockPrevented() const
    {
        return preventAutoLock;
    }

  private:
    bool preventAutoLock;
};

class SMSTemplateSent : public gui::SwitchData
{
  public:
    explicit SMSTemplateSent(const UTF8 &text) : smsText(text){};
    auto getText() -> UTF8
    {
        return smsText;
    }

  private:
    UTF8 smsText;
};

class SMSTextData : public gui::SwitchData
{
  public:
    enum class Concatenate : bool
    {
        True,
        False
    };

    SMSTextData(UTF8 text, Concatenate concatenate = Concatenate::False)
        : text(std::move(text)), concatenate(concatenate)
    {}
    ~SMSTextData() override = default;
    UTF8 text;
    Concatenate concatenate = Concatenate::False;
};

class SMSTemplateRequest : public gui::SwitchData
{
  public:
    SMSTemplateRequest(std::string requestingWindow) : requestingWindow(std::move(requestingWindow))
    {}
    ~SMSTemplateRequest() override = default;

    std::string requestingWindow;
};
