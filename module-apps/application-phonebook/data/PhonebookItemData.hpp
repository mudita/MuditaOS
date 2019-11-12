#pragma once

#include "../PhonebookModel.hpp"
#include "SwitchData.hpp"
#include <string>

class PhonebookItemData : public gui::SwitchData
{
  public:
    PhonebookItemData(std::shared_ptr<ContactRecord> _contact) : contact(_contact){};
    virtual ~PhonebookItemData(){};
    std::shared_ptr<ContactRecord> getContact()
    {
        return contact;
    }

  protected:
    std::shared_ptr<ContactRecord> contact = nullptr;
};

class PhonebookSearchQuery : public gui::SwitchData
{
  public:
    PhonebookSearchQuery(std::string _searchQuery) : searchQuery(_searchQuery){};
    virtual ~PhonebookSearchQuery(){};
    std::string getQuery()
    {
        return (searchQuery);
    }

  protected:
    std::string searchQuery;
};
