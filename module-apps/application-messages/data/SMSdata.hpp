#pragma once

#include "ContactRecord.hpp"
#include "SwitchData.hpp"
#include "ThreadRecord.hpp"
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
  public:
    std::shared_ptr<ContactRecord> contact = nullptr;
    SMSSendRequest(std::shared_ptr<ContactRecord> contact) : contact(contact)
    {}
    virtual ~SMSSendRequest() = default;
};
