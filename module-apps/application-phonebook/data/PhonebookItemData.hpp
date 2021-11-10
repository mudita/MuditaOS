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
  public:
    enum class RequestType
    {
        Internal,
        External
    };

    PhonebookItemData() = default;
    explicit PhonebookItemData(std::shared_ptr<ContactRecord> contact, RequestType requestType = RequestType::Internal)
        : requestType(requestType), contact(std::move(contact)){};

    std::shared_ptr<ContactRecord> getContact() const
    {
        return contact;
    }

    RequestType getRequestType() const noexcept
    {
        return requestType;
    }

  private:
    RequestType requestType                = RequestType::Internal;
    std::shared_ptr<ContactRecord> contact = nullptr;
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

class PhonebookInputOptionData : public gui::SwitchData
{
  private:
    gui::Text *inputText;

  public:
    explicit PhonebookInputOptionData(gui::Text *inputText) : inputText(inputText)
    {}

    gui::Text *getInputText()
    {
        return inputText;
    }
};
