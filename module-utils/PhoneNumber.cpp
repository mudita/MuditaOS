#include "PhoneNumber.hpp"

#include "country.hpp"
#include "log/log.hpp"

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>

#include <stdexcept>
#include <string>

using namespace utils;

PhoneNumber::PhoneNumber(const std::string &phoneNumber, country::Id defaultCountryCode)
    : countryCode(defaultCountryCode), util(*phn_util::GetInstance()), rawInput(phoneNumber)
{
    util.ParseAndKeepRawInput(phoneNumber, country::getAlpha2Code(countryCode), &pbNumber);
}

bool PhoneNumber::isValid() const
{
    return util.IsValidNumber(pbNumber);
}

const std::string &PhoneNumber::get() const
{
    /// raw input can also be returned directly from pbNumber::rawInput() but
    /// unfortunately if number parsing failed raw input is not preserved so
    /// additional field had to be added in order to be able to always return
    /// raw input
    return rawInput;
}

std::string PhoneNumber::getFormatted() const
{
    if (!isValid()) {
        return rawInput;
    }

    std::string formatted;
    util.FormatInOriginalFormat(pbNumber, country::getAlpha2Code(countryCode), &formatted);
    return formatted;
}

std::string PhoneNumber::toE164() const
{
    if (!isValid()) {
        throw std::runtime_error("Can't create E164 format from an invalid number.");
    }

    std::string e164number;
    util.Format(pbNumber, formatType::E164, &e164number);

    return e164number;
}

bool PhoneNumber::operator==(const PhoneNumber &right) const
{
    return util.IsNumberMatch(pbNumber, right.pbNumber) == phn_util::EXACT_MATCH;
}

bool PhoneNumber::operator==(const View &view) const
{
    if (isValid() != view.isValid()) {
        return false;
    }

    return isValid() ? toE164() == view.getE164() : rawInput == view.getEntered();
}

PhoneNumber::numberType PhoneNumber::getType() const
{
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

PhoneNumber::View PhoneNumber::makeView() const
{
    return PhoneNumber::View(get(), getFormatted(), isValid() ? toE164() : "", isValid());
}

PhoneNumber::View PhoneNumber::validateNumber(const std::string &e164, const std::string &entered)
{
    auto &util = *phn_util::GetInstance();

    // parse E164 number
    i18n::phonenumbers::PhoneNumber pbNumberE164;
    if (util.Parse(e164, country::getAlpha2Code(country::Id::UNKNOWN), &pbNumberE164) != errCode::NO_PARSING_ERROR) {
        return PhoneNumber::View(entered, entered, e164, false);
    }

    // get region code from E164 number
    std::string regionCode;
    util.GetRegionCodeForNumber(pbNumberE164, &regionCode);
    if (regionCode == country::getAlpha2Code(country::Id::UNKNOWN)) {
        return PhoneNumber::View(entered, entered, e164, false);
    }

    // use region code to parse number originally entered by the user
    // keep raw input in order to be able to use original format in formatting
    i18n::phonenumbers::PhoneNumber pbNumberEntered;
    if (util.ParseAndKeepRawInput(entered, regionCode, &pbNumberEntered) != errCode::NO_PARSING_ERROR) {
        return PhoneNumber::View(entered, entered, e164, false);
    }

    // check if numbers match
    if (util.IsNumberMatch(pbNumberE164, pbNumberEntered) != phn_util::EXACT_MATCH) {
        return PhoneNumber::View(entered, entered, e164, false);
    }

    // format number to generate full view of the number
    std::string formatted;
    util.FormatInOriginalFormat(pbNumberEntered, regionCode, &formatted);

    return PhoneNumber::View(entered, formatted, e164, true);
}

void PhoneNumber::View::clear()
{
    this->operator=(View());
}

PhoneNumber::View PhoneNumber::viewFromE164(const std::string &e164)
{
    PhoneNumber number(e164, country::Id::UNKNOWN);
    return number.makeView();
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
    return valid == rhs.valid && e164 == rhs.e164;
}
