// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "country.hpp"

#include <phonenumbers/phonenumberutil.h>

#include <exception>
#include <utf8/UTF8.hpp>

namespace utils
{

    /**
     * @brief Phone number representation. It wraps calls to Google's libphonenumber
     * to provide more convienient API. Because operations on phone numbers are
     * substantially resource hungry (in terms of stack and cpu time usage)
     * a lightweight representation of class' data has been provided (View).
     *
     * There are two states of phone number: valid or invalid which means that
     * libphonenumber was or wasn't able to parse provided phone number. It is
     * not possible to determine if the number provided is valid if it is not
     * possible to determine correct country for number so this flag is just a
     * if you can expect E164 number format to be present and meaningful.
     */
    class PhoneNumber
    {
      private:
        using errCode    = ::i18n::phonenumbers::PhoneNumberUtil::ErrorType;
        using formatType = ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat;

      public:
        /**
         * @brief Number type enumeration, e.g. fixed line, mobile, etc.
         */
        using numberType = ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberType;

        /**
         * @brief Phone Number match type:
         *  - EXACT - 100% match (match on E164),
         *  - POSSIBLE - highly likely that numbers match, e.g. one number is in
         *    national format with unknown country code and the other is the
         *    same but has country code.
         *  - PROBABLE - less likely than POSSIBLE but not impossible, e.g. one
         *    number match ending of another number
         *  - NO_MATCH - no match possible
         * number without country code matches same number with country code.
         *
         */
        enum class Match
        {
            NO_MATCH,
            PROBABLE,
            POSSIBLE,
            EXACT
        };

        /**
         * @brief Phone number processing error. Holds reason and string
         * representation of a problematic phone number.
         *
         */
        class Error : public std::exception
        {
          private:
            std::string number;
            std::string reason;

          public:
            Error() = delete;

            /**
             * @brief Construct a new PhoneNumber exception object.
             *
             * @param number - phone number which was processed when an exception occurred
             * @param reason - fail reason
             */
            Error(const std::string &number, const std::string &reason);

            /**
             * @brief Get exception reason
             *
             * @return exception reason
             */
            const char *what() const noexcept override;

            /**
             * @brief Get the phone number related to the exception
             *
             * @return string represenation of a number
             */
            const std::string &getNumber() const;
        };

        /**
         * @brief A lightweight representation of PhoneNumber class. It does not
         * wrap calls to libphonenumber and contains precomputed values only.
         * It is not possible to create an instance of View from outside of
         * PhoneNumber class, which allows to maintain object integrity, i.e.
         * it is not possible to create View with two completely different
         * numbers.
         */
        class View
        {
          public:
            /**
             * @brief Creates empty (null) phone number. It is \a invalid and
             * contains empty strings.
             */
            explicit View() = default;

            /**
             * @brief Default copy constructor.
             *
             * @param view - object to copy from
             */
            View(const View &view) = default;

            /**
             * @brief Default assignment operator.
             *
             * @param view - object to assign from
             * @return View& - reference to self
             */
            View &operator=(const View &view) = default;

            /**
             * @brief Resets state - i.e. turns object into null number (see
             * View()).
             *
             */
            void clear();

            /**
             * @brief A convenience method to get a non empty number.
             *
             * @return E164 number if it has one, entered number otherwise.
             */
            const std::string &getNonEmpty() const;

            /**
             * @brief Getter for the entered number - number parsed from.
             *
             * @return const std::string& - reference to entered number
             */
            const std::string &getEntered() const;

            /**
             * @brief Getter for E164 number representation.
             *
             * @return const std::string& - E164 number representation. Is empty
             * if number is not \a valid (see isValid()).
             */
            const std::string &getE164() const;

            /**
             * @brief Getter for formatted number representation. It is intended
             * to be presented to the user.
             *
             * @return const std::string& - formatted number representation. If
             * number is not \a valid (see isValid()) it is equal to the entered
             * number (see getEntered()).
             */
            const std::string &getFormatted() const;

            /**
             * @brief Object state validator.
             *
             * @return true if number is valid, i.e. when it was successfully
             * parsed using provided country identification.
             *
             * @return false if number is not valid, i.e. when libphonenumber
             * failed to parse number using provided country identification.
             */
            bool isValid() const;

            /**
             * @brief Compares two View objects
             *
             * @param rhs - instance of a VIew to compare to.
             * @return true if objects are equal.
             * @return false if objects are different (not equal).
             */
            bool operator==(const View &rhs) const;

          private:
            View(const std::string &enteredNumber,
                 const std::string &formattedNumber,
                 const std::string &e164Number,
                 bool valid);
            std::string entered   = "";
            std::string formatted = "";
            std::string e164      = "";
            bool valid            = false;

            friend class PhoneNumber;
        };

      private:
        using phn_util = ::i18n::phonenumbers::PhoneNumberUtil;
        using gnumber  = ::i18n::phonenumbers::PhoneNumber;

      public:
        /**
         * @brief Construct an empty PhoneNumber object
         *
         */
        PhoneNumber() = default;

        /**
         * @brief Creates PhoneNumber from a string.
         *
         * @param phoneNumber - phone number provided by the user
         * @param defaultCountryCode - if number is local this country will be
         * used when generating E164 format. If phoneNumber format does not
         * match local number format for the country identified by
         * defaultCountryCode the object will be invalid (see isValid()).
         */
        PhoneNumber(const std::string &phoneNumber, country::Id defaultCountryCode = country::defaultCountry);

        /**
         * @brief Construct a new Phone Number object from its raw and e164
         * representations checking if they match in the process.
         *
         * If e164 is empty PhoneNumber is constructed based on entered number
         * only.
         *
         * Country code is retrieved from E164 number format if possible.
         *
         * @param phoneNumber - phone number provided by the user
         * @param e164number - E164 number representation
         *
         * \throws PhoneNumber::Error when numbers do not match or a parsing error
         * occurs.
         */
        PhoneNumber(const std::string &phoneNumber, const std::string &e164number);

        /**
         * @brief Construct a PhoneNumber object from View representation.
         *
         * @param numberView - lightweight number representation
         */
        PhoneNumber(const View &numberView);

        /**
         * @brief Creates E164 number representation.
         *
         * @return std::string - E164 number representation.
         */
        std::string toE164() const;

        /**
         * @brief Gets orignal number as provided to the constructor.
         *
         * @return const std::string& - number used to construct PhoneNumber
         * instance.
         */
        const std::string &get() const;

        /**
         * @brief Gets string representation of a number formatted for the end
         * user.
         *
         * @return std::string - formatted string.
         */
        std::string getFormatted() const;

        /**
         * @brief Get detected number type (mobile, fixed line, etc.).
         *
         * @return numberType - number type
         */
        numberType getType() const;

        /**
         * @brief Get the country code for number
         *
         * @return country::Id
         */
        country::Id getCountryCode() const noexcept;

        /**
         * @brief Get lightweight representation of a PhoneNumber (see View class).
         *
         * @return View - instance of View object which represents state of
         * PhoneNumber instance.
         */
        const View &getView() const;

        /**
         * @brief Returns validity state, i.e. if number has been successfully
         * parsed when local number had been provided.
         *
         * @return true if number is valid
         * @return false if number is not valid
         */
        bool isValid() const;

        /**
         * @brief Compare current instance with an other number.
         *
         * @param other - number to compare
         * @return type of match, Match::NO_MATCH when number do not match at all
         */
        Match match(const PhoneNumber &other) const;

        /**
         * @brief Compares two PhoneNumber objects (exact match).
         *
         * @param right - instance of PhoneNumber to compare to.
         * @return true if objects are equal.
         * @return false if objects are different (not equal).
         */
        bool operator==(const PhoneNumber &right) const;

        /**
         * @brief Compares PhoneNumber with number represented by View.
         *
         * @param view - instance of View to compare to.
         * @return true if objects are equal.
         * @return false if objects are different (not equal).
         */
        bool operator==(const View &view) const;

        /**
         * @brief Convenience wrapper for E164 number formatting.
         *
         * @param number - number to be formatted
         * @param e164 - where to store the result
         * @param defaultCountryCode - default country code to be used if local
         * number has been provided. Can be country::Id::UNKNOWN if number has
         * been provided with international prefix.
         * @return true if it was possible to format number.
         * @return false if there was an error during number formatting.
         */
        static bool e164format(const std::string &number,
                               std::string &e164,
                               country::Id defaultCountryCode = country::defaultCountry);

        /**
         * @brief Create instance of View directly from the E164 format.
         *
         * @param inputNumber - a string represenation of a number to create View from
         * @return View - phone number representation
         */
        static View parse(const std::string &inputNumber);

        /**
         * @brief Get lightweight representation of a PhoneNumber (see View class).
         *
         * @param received number - utf8 representation of received number
         * @return View - instance of View object which represents state of
         * PhoneNumber instance.
         */
        static const View getReceivedNumberView(const UTF8 &receivedNumber);

      private:
        View makeView(const std::string &input) const;

        country::Id countryCode = country::Id::UNKNOWN;
        gnumber pbNumber;
        View viewSelf;
    };
} // namespace utils
