#pragma once

#include "../models/PhonebookModel.hpp"
#include "ContactRecord.hpp"
#include "SwitchData.hpp"
#include <memory>
#include <string>

class PhonebookItemData : public gui::SwitchData
{
  public:
    PhonebookItemData(std::shared_ptr<ContactRecord> contact, const std::string &text = "") : text(text), contact(contact){};
    virtual ~PhonebookItemData(){};
    std::shared_ptr<ContactRecord> getContact()
    {
        return contact;
    }

  public:
    PhonebookItemData() : contact(nullptr)
    {
    }
    std::string text;
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

class PhonebookSearchReuqest : public gui::SwitchData
{
  public:
    std::string request = "";
    std::shared_ptr<std::vector<ContactRecord>> results = nullptr;
    PhonebookSearchReuqest(std::string request, std::shared_ptr<std::vector<ContactRecord>> results) : request(request), results(results)
    {
    }
    PhonebookSearchReuqest() = default;
    std::shared_ptr<ContactRecord> result = nullptr;
};
