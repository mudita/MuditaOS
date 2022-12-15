// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <time/ScopedTime.hpp>

namespace utils
{

    /**
     * @brief
     *
     * @tparam STL container
     * @tparam Number holder class
     */
    template <template <class, class> class C, class THolder>
    class NumberHolderMatcher
    {
      public:
        using NumbersProvider =
            std::function<std::vector<THolder>(const PhoneNumber &number, unsigned int offset, unsigned int limit)>;

      private:
        using TContainer = C<THolder, std::allocator<THolder>>;
        using TIter      = typename TContainer::const_iterator;
        using OwnType    = NumberHolderMatcher<C, THolder>;

        TContainer numbers;
        NumbersProvider numbersProvider;
        unsigned int currentOffset;
        unsigned int limit;

        auto next(const PhoneNumber &number) -> bool
        {
            currentOffset += limit;
            numbers = numbersProvider(number, currentOffset, limit);
            return !numbers.empty();
        }

        void restartFor(const PhoneNumber &number)
        {
            currentOffset = 0U;
            numbers       = numbersProvider(number, currentOffset, limit);
        }

      public:
        NumberHolderMatcher(NumbersProvider &&provider, unsigned int pageSize)
            : numbers{}, numbersProvider{std::move(provider)}, currentOffset{0U}, limit{pageSize}
        {}

        OwnType &operator=(const OwnType &) = delete;
        OwnType &operator=(OwnType &&m) = default;

        /**
         * @brief Matches a number on a specified level against a vector of numbers via NumberHolder extractor wrapper
         *
         * @param other - number to match with
         * @param level - minimum acceptable match level
         * @param contactIDToIgnore - for this contact ID the match will be ignored
         * @return Best match for the other phone number
         */
        std::optional<THolder> bestMatch(const PhoneNumber &phoneNumber,
                                         PhoneNumber::Match level              = PhoneNumber::Match::EXACT,
                                         const std::uint32_t contactIDToIgnore = 0u)
        {
            utils::time::Scoped t{"bestMatch()"};
            // if empty string, do not try to match, simply return
            if (phoneNumber.get().empty()) {
                return std::nullopt;
            }

            restartFor(phoneNumber);
            do {
                const auto it = std::find_if(
                    numbers.cbegin(), numbers.cend(), [&phoneNumber, level, contactIDToIgnore](const auto &number) {
                        return (phoneNumber.match(number.getNumber()) >= level) &&
                               (number.getContactID() != contactIDToIgnore);
                    });
                if (it != numbers.cend()) {
                    return *it;
                }
            } while (next(phoneNumber));

            return std::nullopt;
        }
    };

}; // namespace utils
