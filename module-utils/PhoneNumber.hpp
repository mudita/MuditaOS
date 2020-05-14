#pragma once

#include "country.hpp"

#include <phonenumbers/phonenumberutil.h>

#include <array>
#include <string>

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
        using errCode    = i18n::phonenumbers::PhoneNumberUtil::ErrorType;
        using formatType = i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat;

      public:
        /**
         * @brief Number type enumeration, e.g. fixed line, mobile, etc.
         */
        using numberType = i18n::phonenumbers::PhoneNumberUtil::PhoneNumberType;

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
            explicit View(const View &view) = default;

            /**
             * @brief Default move constructor.
             *
             * @param view - object to move from
             */
            explicit View(View &&view) = default;

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
        using phn_util = i18n::phonenumbers::PhoneNumberUtil;
        using gnumber  = i18n::phonenumbers::PhoneNumber;

      public:
        PhoneNumber() = delete;

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
         * @brief Creates E164 number representation.
         *
         * @return std::string - E164 number representation.
         * \throws std::runtime_error - tried to get E164 format from a number
         * with \a invalid state (see isValid()).
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
         * @brief Creates lightweight representation of itself (see View class).
         *
         * @return View - instance of View object which represents state of
         * PhoneNumber instance.
         */
        View makeView() const;

        /**
         * @brief Returns validity state, i.e. if number has been successfully
         * parsed when local number had been provided.
         *
         * @return true if number is valid
         * @return false if number is not valid
         */
        bool isValid() const;

        /**
         * @brief Compares two PhoneNumber objects (strict match).
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
         * @brief Create instance of View from a pair of numbers - raw format
         * (as entered by the user) and E164 format. This method is convenient
         * when both numbers has to be stored, e.g. in a database.
         *
         * @param e164 - phone number in the E164 format
         * @param entered - phone number as entered by the user
         * @return View - phone number representation with validity state set.
         */
        static View validateNumber(const std::string &e164, const std::string &entered);

        /**
         * @brief Create instance of View directly from the E164 format.
         *
         * @param e164 - number in the E164 format to create View from
         * @return View - phone number representation
         */
        static View viewFromE164(const std::string &e164);

      private:
        const country::Id countryCode;
        phn_util &util;
        gnumber pbNumber;
        std::string rawInput;
    };
} // namespace utils
