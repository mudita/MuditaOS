// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-phonebook/models/PhonebookModel.hpp>
#include <ContactRecord.hpp>
#include <SwitchData.hpp>

#include <memory>
#include <string>

class PhonebookItemData : public gui::SwitchData
{
    bool externalRequest                   = false;
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookItemData() = default;
    explicit PhonebookItemData(std::shared_ptr<ContactRecord> contact, bool externalRequest = false)
        : externalRequest(externalRequest), contact(std::move(contact)){};

    std::shared_ptr<ContactRecord> getContact() const
    {
        return contact;
    }

    bool getExternalRequest() const noexcept
    {
        return externalRequest;
    }
};

class PhonebookSearchResultsData : public gui::SwitchData
{
  public:
    PhonebookSearchResultsData() = delete;
    PhonebookSearchResultsData(std::unique_ptr<PhonebookModel> model) : model(std::move(model)){};

    std::unique_ptr<PhonebookModel> consumeSearchResultsModel()
    {
        assert(!consumed);
        consumed = true;
        return std::move(model);
    }

  protected:
    std::unique_ptr<PhonebookModel> model;
    bool consumed = false;
};

class PhonebookSearchRequest : public gui::SwitchData
{
  public:
    std::string request                                 = "";
    std::shared_ptr<std::vector<ContactRecord>> results = nullptr;
    PhonebookSearchRequest(std::string request, std::shared_ptr<std::vector<ContactRecord>> results)
        : request(std::move(request)), results(std::move(results))
    {}
    PhonebookSearchRequest()              = default;
    std::shared_ptr<ContactRecord> result = nullptr;
};
