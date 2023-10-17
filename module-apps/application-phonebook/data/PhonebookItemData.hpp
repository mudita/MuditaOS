// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-phonebook/models/PhonebookModel.hpp>
#include <application-phonebook/models/SearchRequestModel.hpp>
#include <ContactRecord.hpp>
#include <SwitchData.hpp>

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
    PhonebookSearchResultsData(std::shared_ptr<PhonebookModel> phonebookModel,
                               std::shared_ptr<SearchRequestModel> searchRequestModel)
        : phonebookModel(std::move(phonebookModel)), searchRequestModel(std::move(searchRequestModel)){};

    std::shared_ptr<PhonebookModel> phonebookModel;
    std::shared_ptr<SearchRequestModel> searchRequestModel;
};

class PhonebookSearchRequest : public gui::SwitchData
{
  public:
    std::string request                                 = "";
    std::shared_ptr<std::vector<ContactRecord>> results = nullptr;
    unsigned int selectedNumber                         = 0;

    PhonebookSearchRequest(std::string request, std::shared_ptr<std::vector<ContactRecord>> results)
        : request(std::move(request)), results(std::move(results))
    {}
    PhonebookSearchRequest()              = default;
    std::shared_ptr<ContactRecord> result = nullptr;
};

class PhonebookMultipleNumbersRequest : public gui::SwitchData
{
  public:
    std::string request                   = "";
    std::shared_ptr<ContactRecord> record = nullptr;

    PhonebookMultipleNumbersRequest(std::string request, std::shared_ptr<ContactRecord> result)
        : request(std::move(request)), record(std::move(result))
    {}
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
