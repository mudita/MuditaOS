// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesRepository.hpp"

#include <module-utils/gsl/gsl_util>
#include <algorithm>
#include <utility>

namespace app
{
    QuotesJsonRepository::QuotesJsonRepository(const std::string &path) : repositoryPath{std::move(path)}
    {}

    void QuotesJsonRepository::get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback)
    {
        if (quotes.empty()) {
            readQuotes(repositoryPath);
        }

        if (callback) {
            std::uint32_t size = quotes.size();
            auto start         = std::next(quotes.begin(), std::min(offset, size));
            auto end           = std::next(quotes.begin(), std::min(offset + limit, size));
            std::list<QuoteRecord> result{start, end};
            callback(result, result.size());
        }
    }

    void QuotesJsonRepository::save(const QuoteRecord &quote)
    {
        auto toEdit = std::find_if(quotes.begin(), quotes.end(), [quote](auto &&d) { return d.id == quote.id; });

        if (toEdit != quotes.end()) {
            toEdit->quote  = quote.quote;
            toEdit->author = quote.author;
        }
        else if (quote.id == 0) {
            quotes.push_back(quote);
        }

        writeQuotes(repositoryPath);
    }

    void QuotesJsonRepository::remove(const QuoteRecord &quote)
    {
        quotes.remove_if([quote](auto &&d) { return d.id == quote.id; });
        writeQuotes(repositoryPath);
    }

    void QuotesJsonRepository::writeQuotes(const fs::path &quotesFilename)
    {
        if (auto file = std::fopen(repositoryPath.c_str(), "w"); file != nullptr) {
            auto _    = gsl::finally([file] { std::fclose(file); });
            auto body = json11::Json{quotes};
            auto text = body.dump();
            std::fwrite(text.c_str(), 1, text.length(), file);
        }
    }

    void QuotesJsonRepository::readQuotes(const fs::path &quotesFilename)
    {
        std::string err;

        const auto fileContents = readFileToString(quotesFilename);
        auto obj                = json11::Json::parse(fileContents.c_str(), err).array_items();

        if (!err.empty()) {
            LOG_ERROR("Error while parsing quotes from file: %s error: %s ", quotesFilename.c_str(), err.c_str());
            return;
        }

        quotes.clear();

        auto id = 1;
        std::transform(obj.begin(), obj.end(), std::back_inserter(quotes), [&id](auto item) {
            return QuoteRecord{id++, item["quote"].string_value(), item["author"].string_value()};
        });
    }

    auto QuotesJsonRepository::readFileToString(const fs::path &filename) -> std::string
    {
        constexpr auto tar_buf = 8192 * 4;
        auto file              = std::fopen(filename.c_str(), "r");
        if (file == nullptr) {
            return {};
        }
        auto _            = gsl::finally([file] { std::fclose(file); });
        const auto length = std::filesystem::file_size(filename);

        if (length >= tar_buf) {
            LOG_ERROR("File %s length is too high!", filename.c_str());
            return {};
        }
        LOG_INFO("file length: %ld", length);
        auto buffer = std::make_unique<char[]>(length + 1);
        std::fread(buffer.get(), 1, length, file);
        return std::string(buffer.get());
    }
} // namespace app
