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

class SMSSendRequest : public gui::SwitchData
{
  protected:
    std::string
        phoneNumber; // once there is support for it in phonebook, it should be switched to utils::PhoneNumber::View

  public:
    SMSSendRequest(const UTF8 &phoneNumber) : phoneNumber(phoneNumber)
    {}
    virtual ~SMSSendRequest() = default;

    const std::string &getPhoneNumber() const
    {
        return phoneNumber;
    };
};

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