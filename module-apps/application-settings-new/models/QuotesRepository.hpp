// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json/json11.hpp>

#include <list>
#include <functional>
#include <filesystem>
#include <module-apps/Application.hpp>

namespace app
{
    struct QuoteRecord
    {
        int id = 0;
        std::string quote;
        std::string author;

        [[nodiscard]] auto to_json() const -> json11::Json
        {
            return json11::Json::object{{"quote", quote}, {"author", author}};
        }
    };

    class QuotesRepository
    {
      public:
        using OnGetCallback = std::function<bool(const std::list<QuoteRecord> &, unsigned int)>;

        virtual ~QuotesRepository() noexcept = default;

        virtual void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) = 0;
        virtual void save(const QuoteRecord &quote)                                                = 0;
        virtual void remove(const QuoteRecord &quote)                                              = 0;
    };

    class QuotesJsonRepository : public QuotesRepository
    {
      public:
        QuotesJsonRepository(const std::string &path);

        void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) override;
        void save(const QuoteRecord &quote) override;
        void remove(const QuoteRecord &quote) override;

      private:
        void writeQuotes(const std::filesystem::path &path);
        void readQuotes(const std::filesystem::path &fn);
        std::string readFileToString(const std::filesystem::path &fn);

        std::list<QuoteRecord> quotes;
        std::string repositoryPath;
    };
} // namespace app
