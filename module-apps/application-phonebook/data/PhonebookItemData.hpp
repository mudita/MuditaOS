#pragma once

#include "../models/PhonebookModel.hpp"
#include "ContactRecord.hpp"
#include "SwitchData.hpp"
#include <memory>
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

class PhonebookSearchResultsData : public gui::SwitchData
{
  public:
    PhonebookSearchResultsData(std::string _searchQuery) : searchQuery(_searchQuery){};
    virtual ~PhonebookSearchResultsData(){};
    void setResults(std::shared_ptr<std::vector<ContactRecord>> _results)
    {
        results = _results;
    }

    std::shared_ptr<std::vector<ContactRecord>> getResults()
    {
        return (results);
    }

    std::string getQuery()
    {
        return (searchQuery);
    }

  protected:
    std::string searchQuery;
    std::shared_ptr<std::vector<ContactRecord>> results;
};
