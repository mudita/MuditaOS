#pragma once

#include <SwitchData.hpp>
#include <SMSTemplateRecord.hpp>
#include <ContactRecord.hpp>
#include <ThreadRecord.hpp>
#include <Database/Database.hpp>
#include <memory>
#include <string>

class SMSThreadData : public gui::SwitchData
{
  public:
    std::shared_ptr<ThreadRecord> thread = nullptr;
    SMSThreadData(std::shared_ptr<ThreadRecord> thread) : thread(thread)
    {}
    virtual ~SMSThreadData() = default;
};

class SMSRequest : public gui::SwitchData
{
  protected:
    utils::PhoneNumber::View phoneNumber;

  public:
    SMSRequest(const utils::PhoneNumber::View &phoneNumber) : phoneNumber(phoneNumber)
    {}
    virtual ~SMSRequest() = default;

    const utils::PhoneNumber::View &getPhoneNumber() const
    {
        return phoneNumber;
    };
};

class SMSSendRequest : public SMSRequest
{
  public:
    SMSSendRequest(const utils::PhoneNumber::View &phoneNumber) : SMSRequest(phoneNumber)
    {}
    virtual ~SMSSendRequest() = default;
};

class SMSSendTemplateRequest : public SMSRequest
{
  public:
    SMSSendTemplateRequest(const utils::PhoneNumber::View &phoneNumber) : SMSRequest(phoneNumber)
    {}
    virtual ~SMSSendTemplateRequest() = default;
};

class SMSTemplateSent : public gui::SwitchData
{};

class SMSTemplateData : public gui::SwitchData
{
  public:
    SMSTemplateData(std::shared_ptr<SMSTemplateRecord> templ) : templ(templ)
    {}
    virtual ~SMSTemplateData()               = default;
    std::shared_ptr<SMSTemplateRecord> templ = nullptr;
};

class SMSTemplateRequest : public gui::SwitchData
{
  public:
    SMSTemplateRequest(const std::string &requestingWindow) : requestingWindow(requestingWindow)
    {}
    virtual ~SMSTemplateRequest() = default;

    std::string requestingWindow;
};