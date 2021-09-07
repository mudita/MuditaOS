// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNumber.hpp"

#include "country.hpp"
#include <log.hpp>

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>

#include <exception>
#include <string>

using namespace utils;

PhoneNumber::Error::Error(const std::string &number, const std::string &reason) : number(number), reason(reason)
{}

const char *PhoneNumber::Error::what() const noexcept
{
    return reason.c_str();
}

const std::string &PhoneNumber::Error::getNumber() const
{
    return number;
}

PhoneNumber::PhoneNumber(const std::string &phoneNumber, country::Id defaultCountryCode)
    : countryCode(defaultCountryCode)
{
    auto &util = *phn_util::GetInstance();
    util.ParseAndKeepRawInput(phoneNumber, country::getAlpha2Code(countryCode), &pbNumber);

    // determine real country code from number
    std::string regionCode;
    util.GetRegionCodeForNumber(pbNumber, &regionCode);
    countryCode = country::getIdByAlpha2Code(regionCode);

    // create view representation
    viewSelf = makeView(phoneNumber);
}

PhoneNumber::PhoneNumber(const View &numberView)
{
    auto &util = *phn_util::GetInstance();

    if (numberView.isValid()) {
        // parse E164 number
        i18n::phonenumbers::PhoneNumber pbNumberE164;
        if (util.Parse(numberView.getE164(), country::getAlpha2Code(country::Id::UNKNOWN), &pbNumberE164) !=
            errCode::NO_PARSING_ERROR) {
            throw PhoneNumber::Error(numberView.getE164(), "Can't parse E164 number");
        }

        // get region code from E164 number
        std::string regionCode;
        util.GetRegionCodeForNumber(pbNumberE164, &regionCode);
        if (regionCode == country::getAlpha2Code(country::Id::UNKNOWN)) {
            throw PhoneNumber::Error(numberView.getE164(), "Can't get country code");
        }
        countryCode = country::getIdByAlpha2Code(regionCode);

        // reparse entered number
        util.ParseAndKeepRawInput(numberView.getEntered(), country::getAlpha2Code(countryCode), &pbNumber);
    }
    else {
        util.ParseAndKeepRawInput(numberView.getEntered(), country::getAlpha2Code(countryCode), &pbNumber);

        // update country code
        std::string regionCode;
        util.GetRegionCodeForNumber(pbNumber, &regionCode);
        countryCode = country::getIdByAlpha2Code(regionCode);
    }

    // save original view without recalculating
    viewSelf = numberView;
}

PhoneNumber::PhoneNumber(const std::string &phoneNumber, const std::string &e164number)
{
    auto &util = *phn_util::GetInstance();
    std::string regionCode;

    // non empty E164 format: match numbers, throw on error
    if (e164number.size() > 0) {
        // parse E164 number
        i18n::phonenumbers::PhoneNumber pbNumberE164;
        if (util.Parse(e164number, country::getAlpha2Code(country::Id::UNKNOWN), &pbNumberE164) !=
            errCode::NO_PARSING_ERROR) {
            throw PhoneNumber::Error(e164number, "Can't parse E164 number");
        }

        // get region code from E164 number
        util.GetRegionCodeForNumber(pbNumberE164, &regionCode);
        if (regionCode == country::getAlpha2Code(country::Id::UNKNOWN)) {
            throw PhoneNumber::Error(e164number, "Can't get country code");
        }

        // update country code information
        countryCode = country::getIdByAlpha2Code(regionCode);

        // use region code to parse number originally entered by the user
        // keep raw input in order to be able to use original format in formatting
        if (util.ParseAndKeepRawInput(phoneNumber, regionCode, &pbNumber) != errCode::NO_PARSING_ERROR) {
            throw PhoneNumber::Error(phoneNumber, "Can't parse phone number");
        }

        // check if numbers match
        if (util.IsNumberMatch(pbNumberE164, pbNumber) != phn_util::EXACT_MATCH) {
            throw PhoneNumber::Error(phoneNumber, "Can't match number with E164 format");
        }
    }
    // empty E164: use entered number
    else {
        util.ParseAndKeepRawInput(phoneNumber, country::getAlpha2Code(countryCode), &pbNumber);

        // determine real country code from number
        util.GetRegionCodeForNumber(pbNumber, &regionCode);
        countryCode = country::getIdByAlpha2Code(regionCode);
    }

    // create view representation
    viewSelf = makeView(phoneNumber);
}

bool PhoneNumber::isValid() const
{
    return viewSelf.isValid();
}

const std::string &PhoneNumber::get() const
{
    return viewSelf.getEntered();
}

std::string PhoneNumber::getFormatted() const
{
    return viewSelf.getFormatted();
}

std::string PhoneNumber::toE164() const
{
    return viewSelf.getE164();
}

PhoneNumber::Match PhoneNumber::match(const PhoneNumber &other) const
{
    auto &util = *phn_util::GetInstance();

    bool valid      = isValid();
    bool otherValid = other.isValid();

    // both numbers are invalid
    if (!valid && !otherValid) {
        if (get() == other.get()) {
            return Match::EXACT;
        }
    }

    // one is valid
    if (valid ^ otherValid) {
        // determine which number is valid and which is not
        const PhoneNumber *validNumber   = nullptr;
        const PhoneNumber *invalidNumber = nullptr;
        if (valid) {
            validNumber   = this;
            invalidNumber = &other;
        }
        else {
            validNumber   = &other;
            invalidNumber = this;
        }

        // longer is E164 because on of the numbers is valid
        // get country from E164 and try to format an invalid to E164
        PhoneNumber invalidCheck(invalidNumber->get(), validNumber->getCountryCode());
        if (validNumber->toE164() == invalidCheck.toE164()) {
            return Match::POSSIBLE;
        }
    }

    // match with libphonenumber if both valid
    if (valid && otherValid) {
        auto matchResult = util.IsNumberMatch(pbNumber, other.pbNumber);
        switch (matchResult) {
        case phn_util::EXACT_MATCH:
            return Match::EXACT;
        case phn_util::SHORT_NSN_MATCH:
        case phn_util::NSN_MATCH:
            return Match::POSSIBLE;

        // fallthrough to last case resort checks
        case phn_util::NO_MATCH:
        case phn_util::INVALID_NUMBER:
            break;
        }
    }

    // try harder than libphonenumber
    if (!valid || !otherValid) {
        // determine longer and shorter number - check if one is an ending of another
        std::string otherEntered = other.getView().getNonEmpty();
        std::string mineEntered  = viewSelf.getNonEmpty();
        auto &longer             = mineEntered.size() > otherEntered.size() ? mineEntered : otherEntered;
        auto &shorter            = mineEntered.size() < otherEntered.size() ? mineEntered : otherEntered;
        auto compareSize         = shorter.size();
        auto compareOffset       = longer.size() - shorter.size();

        if (compareSize > 0 && longer.compare(compareOffset, compareSize, shorter) == 0) {
            return Match::PROBABLE;
        }
    }

    return Match::NO_MATCH;
}

bool PhoneNumber::operator==(const PhoneNumber &right) const
{
    return match(right) == Match::EXACT;
}

bool PhoneNumber::operator==(const View &view) const
{
    if (isValid() != view.isValid()) {
        return false;
    }

    return isValid() ? toE164() == view.getE164() : get() == view.getEntered();
}

PhoneNumber::numberType PhoneNumber::getType() const
{
    auto &util = *phn_util::GetInstance();
    return util.GetNumberType(pbNumber);
}

bool PhoneNumber::e164format(const std::string &number, std::string &e164, country::Id defaultCountryCode)
{
    i18n::phonenumbers::PhoneNumber phNumber;
    auto &util = *phn_util::GetInstance();

    auto result = util.Parse(number, country::getAlpha2Code(defaultCountryCode), &phNumber);
    if (result != errCode::NO_PARSING_ERROR) {
        return false;
    }

    util.Format(phNumber, formatType::E164, &e164);

    return true;
}

PhoneNumber::View PhoneNumber::makeView(const std::string &rawInput) const
{
    auto &util = *phn_util::GetInstance();

    if (!util.IsValidNumber(pbNumber)) {
        return PhoneNumber::View(rawInput, rawInput, "", false);
    }

    std::string formatted;
    util.FormatInOriginalFormat(pbNumber, country::getAlpha2Code(countryCode), &formatted);

    std::string e164number;
    util.Format(pbNumber, formatType::E164, &e164number);

    return PhoneNumber::View(rawInput, formatted, e164number, true);
}

void PhoneNumber::View::clear()
{
    this->operator=(View());
}

PhoneNumber::View PhoneNumber::parse(const std::string &inputNumber)
{
    PhoneNumber number(inputNumber);
    return number.getView();
}

const std::string &PhoneNumber::View::getEntered() const
{
    return entered;
}

const std::string &PhoneNumber::View::getE164() const
{
    return e164;
}

const std::string &PhoneNumber::View::getFormatted() const
{
    return formatted;
}

bool PhoneNumber::View::isValid() const
{
    return valid;
}

PhoneNumber::View::View(const std::string &enteredNumber,
                        const std::string &formattedNumber,
                        const std::string &e164Number,
                        bool valid)
    : entered(enteredNumber), formatted(formattedNumber), e164(e164Number), valid(valid)
{}

bool PhoneNumber::View::operator==(const View &rhs) const
{
    if (valid != rhs.valid) {
        return false;
    }
    return valid ? e164 == rhs.e164 : entered == rhs.entered;
}

const std::string &PhoneNumber::View::getNonEmpty() const
{
    return valid ? e164 : entered;
}

country::Id PhoneNumber::getCountryCode() const noexcept
{
    return countryCode;
}

const PhoneNumber::View &PhoneNumber::getView() const
{
    return viewSelf;
}

const PhoneNumber::View PhoneNumber::getReceivedNumberView(const UTF8 &receivedNumber)
{
    return (receivedNumber.isASCIICombination())
               ? utils::PhoneNumber(receivedNumber.toASCII().value(), utils::country::Id::UNKNOWN).getView()
               : utils::PhoneNumber(receivedNumber, utils::country::Id::UNKNOWN).getView();
}
