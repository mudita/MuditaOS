// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>

namespace utils
{

    /**
     * @brief
     *
     * @tparam STL container
     * @tparam Number holder class
     */
    template <template <class, class> class C, class THolder> class NumberHolderMatcher
    {
      private:
        using TContainer = C<THolder, std::allocator<THolder>>;
        using TIter      = typename TContainer::const_iterator;
        using MatchMap   = std::map<PhoneNumber::Match, std::vector<THolder>>;
        using OwnType    = NumberHolderMatcher<C, THolder>;
        TIter startIterator;

      public:
        /**
         * @brief Default constructor (null object constructor)
         *
         */
        NumberHolderMatcher()
        {}

        /**
         * @brief NumberHolderMatcher range constructor
         *
         * @param cont_begin - start iterator of a container with NumberHolders
         * @param cont_end - end iterator
         */
        NumberHolderMatcher(TIter cont_begin, TIter cont_end) : startIterator(cont_begin), END(cont_end)
        {}

        OwnType &operator=(const OwnType &) = delete;
        OwnType &operator=(OwnType &&m) = default;

        /**
         * @brief Matches a number on a specified level against a vector of numbers via NumberHolder extractor wrapper
         *
         * @param other - number to match with
         * @param level - minimum acceptable match level
         * @return TIter - an iterator instance pointing to best matched element in a container or an end iterator if
         * none could be matched
         */
        TIter bestMatch(const PhoneNumber &other, PhoneNumber::Match level = PhoneNumber::Match::EXACT) const
        {
            TIter i = startIterator;

            while (i != END) {
                auto match = other.match(i->getNumber());
                if (match >= level) {
                    return i;
                }
                i++;
            }

            return END;
        }

        /**
         * @brief end iterator for convienience
         *
         */
        TIter END;
    };

}; // namespace utils
