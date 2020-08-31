#pragma once

#include <SwitchData.hpp>
#include <SMSTemplateRecord.hpp>
#include <ContactRecord.hpp>
#include <ThreadRecord.hpp>
#include <Database/Database.hpp>
#include <memory>
#include <string>
#include <utility>

class SMSThreadData : public gui::SwitchData
{
  public:
    std::shared_ptr<ThreadRecord> thread = nullptr;
    SMSThreadData(std::shared_ptr<ThreadRecord> thread) : thread(std::move(thread))
    {}
    ~SMSThreadData() override = default;
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
    SMSSendTemplateRequest(const utils::PhoneNumber::View &phoneNumber) : SMSRequest(phoneNumber)
    {}
    ~SMSSendTemplateRequest() override = default;
};

class SMSTemplateSent : public gui::SwitchData
{};

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
