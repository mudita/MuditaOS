// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcCusd.hpp"
#include <magic_enum.hpp>
#include <re2/re2.h>

using namespace at::urc;

auto Cusd::isURC(const std::string &uHead) -> bool
{
    return uHead == head;
}

auto Cusd::Handle(UrcHandler &h) -> void
{
    h.Handle(*this);
}

Cusd::Cusd(const std::string &urcBody, const std::string &urcHead) : Urc(urcBody, urcHead)
{
    try {
        split(urcBody);

        // MOS-858: decide whether to try to display anything or to bail out
        int constexpr supportedAlphabets[]{0, 15};
        if (auto dcs = getDCS();
            dcs.has_value() && std::find(std::begin(supportedAlphabets), std::end(supportedAlphabets), *dcs) ==
                                   std::end(supportedAlphabets)) {
            throw std::runtime_error("unsupported CUSD alphabet");
        }

        if (auto status = getStatus();
            status != StatusType::FurtherUserActionRequired && status != StatusType::NoFurtherUserActionRequired) {
            LOG_WARN("unsupported CUSD status: %d", magic_enum::enum_integer(status));
        }
    }
    catch (std::runtime_error const &exc) {
        valid_ = false;
        LOG_ERROR("%s", exc.what());
    }
}

auto Cusd::isValid() const noexcept -> bool
{
    return valid_ && Urc::isValid();
}

auto Cusd::isActionNeeded() const noexcept -> bool
{
    return getStatus() == StatusType::FurtherUserActionRequired;
}

auto Cusd::getMessage() const noexcept -> std::optional<std::string>
{
    if (!isValid()) {
        return std::nullopt;
    }

    if (auto const &messageToken = tokens[Tokens::Response]; !messageToken.empty()) {
        return std::make_optional(std::move(messageToken));
    }

    return std::nullopt;
}

auto Cusd::getStatus() const noexcept -> StatusType
{
    return *magic_enum::enum_cast<StatusType>(std::stoi(tokens[Tokens::Status]));
}

auto Cusd::getDCS() const noexcept -> std::optional<int>
{
    if (auto const &dcsToken = tokens[Tokens::DCS]; !dcsToken.empty()) {
        return std::make_optional(std::stoi(dcsToken));
    }

    return std::nullopt;
}

auto Cusd::split(const std::string &str) -> void
{
    size_t constexpr maxNumberOfDcsTokens = 3;
    tokens.resize(maxNumberOfDcsTokens);

    using namespace re2;
    re2::StringPiece input(str);

    auto constexpr numberOfStatusTypes = magic_enum::enum_count<StatusType>();
    static_assert(numberOfStatusTypes <= 9,
                  "StatusType: too many enum entries to handle - please revise regex/algorithm");
    std::string regexForStatus(" ([0-" + std::to_string(numberOfStatusTypes) + "])");

    if (!RE2::Consume(&input, regexForStatus, &tokens[Tokens::Status])) {
        throw std::runtime_error("unrecognized CUSD status field or corrupted CUSD format");
    }

    auto noFurtherInput = [&input]() { return input == "\r\n"; };

    if (noFurtherInput()) {
        return;
    }

    size_t startQuotationMarkPosition = input.find('"');
    // must give the size to rfind() because called in the default way is broken
    size_t endQuotationMarkPosition = input.rfind('"', input.size());
    if (startQuotationMarkPosition == StringPiece::npos || endQuotationMarkPosition == StringPiece::npos) {
        throw std::runtime_error("cannot locate message - corrupted CUSD format");
    }

    if (endQuotationMarkPosition == startQuotationMarkPosition + 1) {
        LOG_WARN("empty CUSD message");
    }
    else {
        auto messageStartPosition = startQuotationMarkPosition + 1;
        auto messageEndPosition   = endQuotationMarkPosition;
        auto messageLength        = messageEndPosition - messageStartPosition;
        tokens[Tokens::Response]  = input.substr(messageStartPosition, messageLength).as_string();
    }

    input.remove_prefix(endQuotationMarkPosition + 1);
    if (noFurtherInput()) {
        return;
    }

    if (!RE2::FullMatch(input, ",([0-9]+)\r\n", &tokens[Tokens::DCS])) {
        throw std::runtime_error("corrupted CUSD format");
    }
}
