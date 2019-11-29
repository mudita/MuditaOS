/*
 * E164 Type base functions
 * $Id: e164_base.c 53 2007-09-10 01:13:48Z glaesema $
 *
# E.164 Telephone Number Type

WARNING: This is an *ALPHA* release. Do *not* run this software in production.

E.164 is a recommendation by the International Telecommunication Union (ITU) for
international telephone numbers. The E164 type provides a convenient method of
storing international telephone numbers consistent with the E.164 recommendation.
In particular,
  * Country code validation
  * Rudimentary format checking
    - Number consists of proper + prefix followed by digits.
    - Minimum and maximum length checking for corresponding E.164 Type

The E164 type does not check that the number is consistent with formats specific
to particular national standards: formats vary by country. (Support for national
format checking may be added in a future release.)

## TODO

* Add ENUM output (and possibly input) functions
* Properly handle error conditions.
* Fix issues raising compiler warnings:
e164.c: In function 'e164_in':
e164.c:146: warning: 'theNumber' may be used uninitialized in this function
e164.c:156: warning: control reaches end of non-void function
e164_base.c: In function 'e164In':
e164_base.c:553: warning: 'theNumber' may be used uninitialized in this function
e164_base.c:561: warning: control reaches end of non-void function
* Fix all other FIXMEs in code :)

## Copyright and License
Copyright (c) 2007-2011, Michael Glaesemann
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef E164_BASE_H
#define E164_BASE_H

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define E164_PREFIX '+'

#define E164_NUMBER_MASK 0x0000FFFFFFFFFFFFULL
#define E164_CC_LENGTH_MASK 0x0003000000000000ULL
#define E164_NUMBER_AND_CC_LENGTH_MASK (E164_NUMBER_MASK | E164_CC_LENGTH_MASK)
#define E164_ONE_DIGIT_CC_MASK 0x0001000000000000ULL
#define E164_TWO_DIGIT_CC_MASK 0x0002000000000000ULL
#define E164_THREE_DIGIT_CC_MASK 0x0003000000000000ULL
#define E164_CC_LENGTH_OFFSET 48

#define E164_GEOGRAPHIC_AREA_MASK 0x0010000000000000ULL
#define E164_GLOBAL_SERVICE_MASK 0x0020000000000000ULL
#define E164_NETWORK_MASK 0x0040000000000000ULL
#define E164_GROUP_OF_COUNTRIES_MASK 0x0080000000000000ULL

/*
 * FIXME
 * These debug symbols should *not* be used in production
 * code, so should not be required here.
 */
typedef enum E164DebugSignal
{
    E164DebugSignalE164CountryCodeFromString_bad_CC = 1,
    E164DebugSignalE164CountryCodeFromString_invalid_CC,
    E164DebugSignalHasValidLengthForE164Type_SN_LESS_THAN_ZERO,
    E164DebugSignalHasValidLengthFore164Type_INVALID_E164TYPE,
    E164DebugSignalCheckE164CountryCodeForRangeError,
    E164DebugSignalE164In,
    E164DebugSignalInitializeE164WithCountryCode,
    E164DebugSignalE164CountryCodeLength,
    E164DebugSignalInitializeE164WithCountryCodeUnrecognizedType,
    E164DebugSignalE164Type
} E164DebugSignal;

/*
 * There are four types of assigned E164:
 *    * Geographic Area numbers
 *    * Global Service numbers
 *    * Network numbers
 *    * Group of Countries numbers
 * Each of these E164 number types have well-defined formats.
 *
 * There are three types of unassigned E164 as well:
 *     * Reserved numbers
 *     * Spare codes with notes
 *     * Spare codes without notes
 *
 * For the purposes of the implementation, all unassigned E164 numbers
 * will be considered invalid.
 *
 * An E164's type can be determined by inspecting its country code --
 * (at most) the first three digits. (Some Geographic Area country codes are one
 * or 2 digits in length.)
 *
 * Some country codes are reserved, and others are spare (as of yet unassigned).
 * These reserved and spare codes are rejected as invalid by this implementation,
 * in part because one cannot determine such a number's type and therefore whether
 * the number matches the format of its type.
 */

/*
 * Type definitions
 */

typedef enum E164StructureLimit
{
    E164MaximumNumberOfDigits = 15,
    E164PrefixStringLength = 1, /* = strlen(E164_PREFIX) */
                                /*
                                 * E164MaxStringLength        = E164MaximumNumberOfDigits + E164PrefixStringLength
                                 * Note this does *not* include the string terminator
                                 */
    E164MaximumStringLength = 16,
    /*
     * E164MinimumStringLength is pretty conservative:
     * prefix (1) + country code (1) + subscriber number (1)
     */
    E164MinimumStringLength = 3,
    E164MinimumNumberOfDigits = 2,

    E164MaximumCountryCodeLength = 3,
    E164GeographicAreaMinimumCountryCodeLength = 1,
    E164GeographicAreaMaximumCountryCodeLength = 3, /* = E164MaximumCountryCodeLength */
    E164GlobalServiceCountryCodeLength = 3,         /* = E164MaximumCountryCodeLength */
    E164NetworkCountryCodeLength = 3,               /* = E164MaximumCountryCodeLength */
    E164GroupOfCountrysCountryCodeLength = 3,       /* = E164MaximumCountryCodeLength */

    /*
     * Minimum Subscriber Number lengths for the various E164Types
     * These are absolute (and unrealistic) minimums. However, true
     * minimums are country specific, and until this implementation
     * is country-code specific, this should do.
     */
    E164GeographicAreaMinimumSubscriberNumberLength = 1,
    E164GlobalServiceMinimumSubscriberNumberLength = 1,
    E164NetworkMinimumSubscriberNumberLength = 2,
    E164GroupOfCountriesMinimumSubscriberNumberLength = 2
} E164StructureLimit;

typedef enum E164ParseResult
{
    E164NoParseError = 0,
    E164ParseErrorBadFormat = 2,
    E164ParseErrorInvalidPrefix,
    E164ParseErrorStringTooLong,
    E164ParseErrorStringTooShort,
    E164ParseErrorInvalidType,
    E164ParseErrorNoSubscriberNumberDigits,
    E164ParseErrorUnassignedType,
    E164ParseErrorTypeLengthMismatch
} E164ParseResult;

typedef enum E164Type
{
    E164GeographicArea,
    E164GlobalService,
    E164Network,
    E164GroupOfCountries,
    /* Unassigned codes */
    E164Reserved,
    E164SpareWithNote,
    E164SpareWithoutNote,
    E164Invalid
} E164Type;

typedef int E164CountryCode;
typedef uint64_t E164;

/*
static const char * e164TypeName[] = {
    "E.164 Geographic Area",
    "E.164 Global Service",
    "E.164 Network",
    "E.164 Group of Countries",
    "E.164 Reserved",
    "E.164 Spare with Note",
    "E.164 Spare without Note",
    "E.164 Invalid"
};
*/

static const E164Type e164TypeFor[] = {
    /* 0..9 */
    E164Reserved, E164GeographicArea, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164GeographicArea, E164Invalid, E164Invalid,
    /* 10..19 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 20..29 */
    E164GeographicArea, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164GeographicArea, E164Invalid, E164Invalid,
    /* 30..39 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164Invalid, E164GeographicArea, E164Invalid,
    E164Invalid, E164GeographicArea,
    /* 40..49 */
    E164GeographicArea, E164GeographicArea, E164Invalid, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea,
    /* 50..59 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164Invalid,
    /* 60..69 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164Invalid,
    E164Invalid, E164Invalid,
    /* 70..79 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 80..89 */
    E164Invalid, E164GeographicArea, E164GeographicArea, E164Invalid, E164GeographicArea, E164Invalid, E164GeographicArea, E164Invalid, E164Invalid,
    E164Invalid,
    /* 90..99 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164Invalid, E164Invalid,
    E164GeographicArea, E164Invalid,
    /* 100..109 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 110..119 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 120..129 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 130..139 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 140..149 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 150..159 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 160..169 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 170..179 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 180..189 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 190..199 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,

    /* 200..209 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 210..219 */
    E164SpareWithoutNote, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164SpareWithoutNote, E164GeographicArea,
    E164SpareWithoutNote, E164GeographicArea, E164SpareWithoutNote,
    /* 220..229 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 230..239 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 240..249 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 250..259 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164SpareWithoutNote,
    /* 260..269 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 270..279 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 280..289 */
    E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote,
    E164SpareWithNote, E164SpareWithNote,
    /* 290..299 */
    E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,

    /* 300..309 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 310..319 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 320..329 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 330..339 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 340..349 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 350..359 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 360..369 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 370..379 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 380..389 */
    E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GroupOfCountries, E164GeographicArea,
    /* 390..399 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,

    /* 400..409 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 410..419 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 420..429 */
    E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164GeographicArea, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,
    E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,
    /* 430..439 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 440..449 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 450..459 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 460..469 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 470..479 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 480..489 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 490..499 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,

    /* 500..509 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 510..519 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 520..529 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 530..539 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 540..549 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 550..559 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 560..569 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 570..579 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 580..589 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 590..599 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,

    /* 600..609 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 610..619 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 620..629 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 630..639 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 640..649 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 650..659 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 660..669 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 670..679 */
    E164GeographicArea, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 680..689 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164GeographicArea,
    /* 690..699 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,
    E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,

    /* 700..709 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 710..719 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 720..729 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 730..739 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 740..749 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 750..759 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 760..769 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 770..779 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 780..789 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 790..799 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,

    /* 800..809 */
    E164GlobalService, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote,
    E164GlobalService, E164SpareWithNote,
    /* 810..819 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 820..829 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 830..839 */
    E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote,
    E164SpareWithNote, E164SpareWithNote,
    /* 840..849 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 850..859 */
    E164GeographicArea, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea, E164SpareWithoutNote, E164GeographicArea, E164GeographicArea,
    E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote,
    /* 860..869 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 870..879 */
    E164Network, E164Network, E164Network, E164Network, E164Reserved, E164Reserved, E164Reserved, E164Reserved, E164GlobalService, E164Reserved,
    /* 880..889 */
    E164GeographicArea, E164Network, E164Network, E164SpareWithoutNote, E164SpareWithoutNote, E164SpareWithoutNote, E164Reserved, E164SpareWithoutNote,
    E164Reserved, E164SpareWithoutNote,
    /* 890..899 */
    E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote, E164SpareWithNote,
    E164SpareWithNote, E164SpareWithNote,

    /* 900..909 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 910..919 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 920..929 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 930..939 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 940..949 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 950..959 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 960..969 */
    E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164GeographicArea, E164GeographicArea, E164Reserved,
    /* 970..979 */
    E164Reserved, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164SpareWithoutNote, E164GlobalService,
    /* 980..989 */
    E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid, E164Invalid,
    /* 990..999 */
    E164SpareWithoutNote, E164GlobalService, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea, E164GeographicArea,
    E164SpareWithoutNote, E164GeographicArea, E164Reserved};

/*
 * Function prototypes
 */

E164 e164In(char *theString);
char *e164Out(E164 aNumber);

static inline E164Type e164Type(E164 *theNumber);
static inline E164CountryCode e164CountryCode(E164 *theNumber);
static inline int countryCodeStringFromE164(char *aString, E164 *aNumber);
static inline int e164CountryCodeLength(E164 *aNumber);
static inline E164CountryCode e164CountryCodeFromInteger(int anInteger);
static inline E164CountryCode e164CountryCodeFromString(char *aString);
static int parseE164String(const char *aNumberString, char *theDigits, E164CountryCode *aCode, char *subscriberNumber);
static inline bool hasValidLengthForE164Type(int numberLength, int countryCodeLength, E164Type aType);
static inline void initializeE164WithCountryCode(E164 *aNumber, E164CountryCode *aCountryCode);
static bool e164IsConsistent(E164 *aNumber);

/*
 * unused, but perhaps useful as faster than converting country codes to text
 * and comparing.
 */
static inline bool e164CountryCodesAreEqual(E164CountryCode firstCountryCode, E164CountryCode secondCountryCode);
/* unused */
static inline bool e164TypesAreEqual(E164Type firstType, E164Type secondType);

static inline bool isValidE164PrefixChar(const char *aChar);
static inline bool stringHasValidE164Prefix(const char *aString);

static inline E164ParseResult e164FromString(E164 *aNumber, char *aString);
static inline int stringFromE164(char *aString, E164 *aNumber, int stringLength);
static inline void e164Free(E164 *theNumber);

static inline bool isEndOfString(const char *theChar);
static inline bool eachCharIsDigit(const char *aString);

static inline bool e164CountryCodeIsInRange(E164CountryCode theCountryCode);
static inline void checkE164CountryCodeForRangeError(E164CountryCode theCountryCode);

static inline bool isValidE164CountryCodeForType(E164CountryCode theCountryCode, E164Type theType);
static inline bool isValidE164CountryCodeForTypeWithDigits(E164CountryCode theCountryCode, E164Type theType, int numberOfDigits);
static inline bool isOneDigitE164CountryCode(E164CountryCode theCountryCode);
static inline bool isTwoDigitE164CountryCode(E164CountryCode theCountryCode);
static inline bool isThreeDigitE164CountryCode(E164CountryCode theCountryCode);
static inline bool isUnassignedE164Type(E164Type aType);
static inline bool isInvalidE164Type(E164Type aType);
static inline E164Type e164TypeForCountryCode(E164CountryCode theCountryCode);

/*
 * Comparison functions
 */
static inline int e164Comparison(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsEqualTo(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsNotEqualTo(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsLessThan(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsLessThanOrEqualTo(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsGreaterThanOrEqualTo(E164 *firstNumber, E164 *secondNumber);
static inline bool e164IsGreaterThan(E164 *firstNumber, E164 *secondNumber);

/*
 * e164Free frees the memory allocated for an E164 variable
 */
void e164Free(E164 *theNumber)
{
    free(theNumber);
    return;
}

static inline int e164Comparison(E164 *firstNumber, E164 *secondNumber)
{
    if ((*firstNumber & E164_NUMBER_AND_CC_LENGTH_MASK) < (*secondNumber & E164_NUMBER_AND_CC_LENGTH_MASK))
        return -1;
    else if ((*firstNumber & E164_NUMBER_AND_CC_LENGTH_MASK) == (*secondNumber & E164_NUMBER_AND_CC_LENGTH_MASK))
        return 0;
    else
        return 1;
};
static inline bool e164IsEqualTo(E164 *firstNumber, E164 *secondNumber)
{
    return (0 == e164Comparison(firstNumber, secondNumber));
};
static inline bool e164IsNotEqualTo(E164 *firstNumber, E164 *secondNumber)
{
    return (0 != e164Comparison(firstNumber, secondNumber));
}
static inline bool e164IsLessThan(E164 *firstNumber, E164 *secondNumber)
{
    return (0 > e164Comparison(firstNumber, secondNumber));
}
static inline bool e164IsLessThanOrEqualTo(E164 *firstNumber, E164 *secondNumber)
{
    return (0 >= e164Comparison(firstNumber, secondNumber));
}
static inline bool e164IsGreaterThanOrEqualTo(E164 *firstNumber, E164 *secondNumber)
{
    return (0 <= e164Comparison(firstNumber, secondNumber));
}
static inline bool e164IsGreaterThan(E164 *firstNumber, E164 *secondNumber)
{
    return (0 < e164Comparison(firstNumber, secondNumber));
}

/*
 * e164Type returns the E164Type of the E164 argument
 */
static inline E164Type e164Type(E164 *theNumber)
{
    if (*theNumber & E164_GEOGRAPHIC_AREA_MASK)
        return E164GeographicArea;
    else if (*theNumber & E164_GLOBAL_SERVICE_MASK)
        return E164GlobalService;
    else if (*theNumber & E164_NETWORK_MASK)
        return E164Network;
    else if (*theNumber & E164_GROUP_OF_COUNTRIES_MASK)
        return E164GroupOfCountries;
}

/*
 * e164CountryCode returns the E164CountryCode for the E164 argument
 */
static inline E164CountryCode e164CountryCode(E164 *theNumber)
{
    E164CountryCode aCountryCode;
    char *aCountryCodeString = (char *)malloc(E164MaximumCountryCodeLength + 1);
    countryCodeStringFromE164(aCountryCodeString, theNumber);
    aCountryCode = e164CountryCodeFromString(aCountryCodeString);
    free(aCountryCodeString);
    return aCountryCode;
}

/*
 * countryCodeStringFromE164 assigns the country code for aNumber to aString, returning
 * the number of characters written (in this case, the number of digits in the
 * country code).
 */
static inline int countryCodeStringFromE164(char *aString, E164 *aNumber)
{
    return snprintf(aString, e164CountryCodeLength(aNumber) + 1, "%lld", (*aNumber & E164_NUMBER_MASK));
}

/*
 * e164CountryCodeLength returns the length of the country code for an E164 number
 */
static inline int e164CountryCodeLength(E164 *aNumber)
{
    return (int)((*aNumber & E164_CC_LENGTH_MASK) >> E164_CC_LENGTH_OFFSET);
}

/*
 * e164CountryCodeFromInteger returns the E164CountryCode represented by anInteger.
 * An error is raised if anInteger is out of the range for E164CountryCode values.
 */

static inline E164CountryCode e164CountryCodeFromInteger(int anInteger)
{
    E164CountryCode theCountryCode;
    theCountryCode = (E164CountryCode)anInteger;
    checkE164CountryCodeForRangeError(theCountryCode);
    return theCountryCode;
}

/*
 * e164CountryCodeFromString returns the E164CountryCode represented by aString.
 * An error is raised if aString does not represent a valid E164CountryCode value.
 */
static inline E164CountryCode e164CountryCodeFromString(char *aString)
{
    int anInteger;
    E164CountryCode theCountryCode;
    if ((E164MaximumCountryCodeLength >= strlen(aString)) && eachCharIsDigit(aString))
    {
        anInteger = atoi(aString);
        theCountryCode = (E164CountryCode)anInteger;
        if (e164CountryCodeIsInRange(theCountryCode))
            return theCountryCode;
        else
        {
            return (-1);
        }
    }
    else
    {
        return (-1);
    }
}

/*
 * stringFromE164 assigns the string representation of aNumber to aString
 */
static inline int stringFromE164(char *aString, E164 *aNumber, int stringLength)
{
    return snprintf(aString, stringLength, "+%lld", (*aNumber & E164_NUMBER_MASK));
}

/*
 * e164CountryCodesAreEqual tests firstCountryCode and secondCountryCode for equality
 */

static inline bool e164CountryCodesAreEqual(E164CountryCode firstCountryCode, E164CountryCode secondCountryCode)
{
    return (firstCountryCode == secondCountryCode);
}

static inline bool e164TypesAreEqual(E164Type firstType, E164Type secondType)
{
    return (firstType == secondType);
}

/*
 * isValidE164PrefixChar returns true if theChar is a valid E164 prefix character
 * and false otherwise.
 */
static inline bool isValidE164PrefixChar(const char *aChar)
{
    return (E164_PREFIX == *aChar);
}

/*
 * stringHasValidE164Prefix returns true if aString has a valid E164 prefix
 * and false otherwise.
 */
static inline bool stringHasValidE164Prefix(const char *aString)
{
    return isValidE164PrefixChar(aString);
}

/*
 * e164FromString takes a string as its second argument and
 * assigns to its first argument the E164 value represented
 * by the string. If the assignment is successful, e164FromString
 * returns true, and false otherwise.
 */
static inline E164ParseResult e164FromString(E164 *aNumber, char *aString)
{
    char *theDigits = (char *)malloc(E164MaximumStringLength);
    E164CountryCode *countryCode = (E164CountryCode *)malloc(sizeof(E164CountryCode));
    char *subscriberNumber = (char *)malloc(E164MaximumStringLength);
    E164ParseResult e164ParseResult;

    e164ParseResult = (E164ParseResult)parseE164String(aString, theDigits, countryCode, subscriberNumber);
    free(subscriberNumber);
    if (E164NoParseError == e164ParseResult)
    {
        E164 e164Result;
        char *cp;
        initializeE164WithCountryCode(&e164Result, countryCode);
        e164Result = (e164Result | strtoll(theDigits, &cp, 10));
        *aNumber = e164Result;
    }
    free(countryCode);
    free(theDigits);
    return e164ParseResult;
}

/*
 * parseE164String parses aString meant to represent an E164 number, assigning the
 * digits of the E164 number to theDigits, the E164CountryCode to the aCode parameter,
 * and the remainder of the number to subscriberNumber. parseE164String returns
 * E164NoParseError on no error, or an error code describing the error encountered.
 */
static int parseE164String(const char *aNumberString, char *theDigits, E164CountryCode *aCode, char *subscriberNumber)
{
    const int stringLength = strlen(aNumberString);
    const char *endOfString = aNumberString + stringLength;
    const char *theChar = aNumberString;
    const char *remainder;
    /*
     * Make sure string doesn't exceed maximum length
     */
    if (E164MaximumStringLength < stringLength)
        return E164ParseErrorStringTooLong;
    if (E164MinimumStringLength > stringLength)
        return E164ParseErrorStringTooShort;
    /*
     * Check for a valid E164 prefix
     */
    if (!stringHasValidE164Prefix(aNumberString))
        return E164ParseErrorInvalidPrefix;

    /* Advance past prefix character */
    theChar++;
    remainder = theChar;
    if (eachCharIsDigit(remainder))
    {
        /*
         * If the remainder of the string is all digits,
         * then the remainder can be (potentially) used for the E164 number.
         */

        int numberOfCountryCodeDigits = 0;
        int digitIndex;
        char *countryCodeString = (char *)malloc(E164MaximumCountryCodeLength + 1);
        E164CountryCode aCountryCode;
        E164Type theType = E164Invalid;

        strcpy(theDigits, remainder);

        for (digitIndex = 1; E164MaximumCountryCodeLength >= digitIndex; digitIndex++)
        {
            E164Type aType;
            if (endOfString <= (remainder + digitIndex))
            {
                free(countryCodeString);
                return E164ParseErrorNoSubscriberNumberDigits;
            }
            strncpy(countryCodeString, remainder, digitIndex);
            countryCodeString[digitIndex] = '\0';
            aCountryCode = e164CountryCodeFromString(countryCodeString);
            aType = e164TypeForCountryCode(aCountryCode);
            if (E164Invalid != aType)
            {
                numberOfCountryCodeDigits = digitIndex;
                theType = aType;
                break;
            }
        }
        free(countryCodeString);
        /*
         * Assign country code.
         * If it's invalid, it'll be used in the error message.
         */
        *aCode = aCountryCode;
        if (isInvalidE164Type(theType))
            return E164ParseErrorInvalidType;
        else if (isUnassignedE164Type(theType))
            return E164ParseErrorUnassignedType;
        /*
         * Need some digits for the subscriber number
         */
        if ((int)strlen(remainder) <= numberOfCountryCodeDigits)
            return E164ParseErrorNoSubscriberNumberDigits;
        /*
         * Check number against E164Type
         * This tests against abolute (and unrealistic) minimums.
         * See comment regarding minimum Subscriber Number lengths
         */
        if (hasValidLengthForE164Type(strlen(remainder), numberOfCountryCodeDigits, theType))
            return E164NoParseError;
        else
            return E164ParseErrorTypeLengthMismatch;
    }
    else
        return E164ParseErrorBadFormat;
}

/*
 * hasValidLengthForE164Type returns true if the number of digits in
 * the number is consistent with its E164Type and E164CountryCode
 */
static inline bool hasValidLengthForE164Type(int numberLength, int countryCodeLength, E164Type aType)
{
    int subscriberNumberLength = (numberLength - countryCodeLength);
    if (0 == subscriberNumberLength)
        return false;
    switch (aType)
    {
    case E164GeographicArea:
        return (E164GeographicAreaMinimumSubscriberNumberLength <= subscriberNumberLength);
        break;
    case E164GlobalService:
        return (E164GlobalServiceMinimumSubscriberNumberLength <= subscriberNumberLength);
        break;
    case E164Network:
        return (E164NetworkMinimumSubscriberNumberLength <= subscriberNumberLength);
        break;
    case E164GroupOfCountries:
        return (E164GroupOfCountriesMinimumSubscriberNumberLength <= subscriberNumberLength);
        break;
    default:
        break;
    }
    return (false);
}

/*
 * intializeE164WithCountryCode intializes aNumber with the length of
 * the countryCode
 */
static inline void initializeE164WithCountryCode(E164 *aNumber, E164CountryCode *aCountryCode)
{
    /*
     * Only Geographic Areas have country codes with lengths 1 or 2.
     */
    if (isOneDigitE164CountryCode(*aCountryCode))
        *aNumber = E164_ONE_DIGIT_CC_MASK | E164_GEOGRAPHIC_AREA_MASK;
    else if (isTwoDigitE164CountryCode(*aCountryCode))
        *aNumber = E164_TWO_DIGIT_CC_MASK | E164_GEOGRAPHIC_AREA_MASK;
    else if (isThreeDigitE164CountryCode(*aCountryCode))
    {
        uint64_t typeMask = 0;
        E164Type aType = e164TypeForCountryCode(*aCountryCode);
        switch (aType)
        {
        case E164GeographicArea:
            typeMask = E164_GEOGRAPHIC_AREA_MASK;
            break;
        case E164GlobalService:
            typeMask = E164_GLOBAL_SERVICE_MASK;
            break;
        case E164Network:
            typeMask = E164_NETWORK_MASK;
            break;
        case E164GroupOfCountries:
            typeMask = E164_GROUP_OF_COUNTRIES_MASK;
            break;
        default:
            break;
        }
        *aNumber = E164_THREE_DIGIT_CC_MASK | typeMask;
    }
}

static inline bool e164IsConsistent(E164 *aNumber)
{
    return (*aNumber == e164In(e164Out(*aNumber)));
}

/*
 * isEndOfString determines if the argument is the end of string character
 */
static inline bool isEndOfString(const char *theChar)
{
    return ('\0' == *theChar);
}

/*
 * eachCharIsDigit returns true if every character of aString is a digit,
 * and false otherwise.
 */
static inline bool eachCharIsDigit(const char *aString)
{
    const char *theChar = aString;
    bool isAllDigits = true;
    while (!isEndOfString(theChar))
    {
        if (!isdigit(*theChar))
        {
            isAllDigits = false;
            break;
        }
        theChar++;
    }
    return isAllDigits;
}

/*
 * e164CountryCodeIsInRange returns true if the E164CountryCode argument is
 * within the proper range for E164CountryCodes and false otherwise.
 */
static inline bool e164CountryCodeIsInRange(E164CountryCode theCountryCode)
{
    return ((0 <= theCountryCode) && (999 >= theCountryCode));
}

/*
 * checkE164CountryCodeForRangeError raises an error if the E164CountryCode
 * argument is out of range.
 */
static inline void checkE164CountryCodeForRangeError(E164CountryCode theCountryCode)
{
    e164CountryCodeIsInRange(theCountryCode);
}

/*
 * isValidE164CountryCodeForType determines if theCountryCode is
 * valid for theType.
 */
static inline bool isValidE164CountryCodeForType(E164CountryCode theCountryCode, E164Type theType)
{
    E164Type countryCodeType;
    countryCodeType = e164TypeForCountryCode(theCountryCode);
    return (countryCodeType == theType);
}

/*
 * isValidE164CountryCodeForTypeWithDigits determines if theCountryCode is valid
 * for theType and has the specified numberOfDigits
 */
static inline bool isValidE164CountryCodeForTypeWithDigits(E164CountryCode theCountryCode, E164Type theType, int numberOfDigits)
{
    switch (numberOfDigits)
    {
    case 1:
        if (!isOneDigitE164CountryCode(theCountryCode))
            return false;
        break;
    case 2:
        if (!isTwoDigitE164CountryCode(theCountryCode))
            return false;
        break;
    case 3:
        if (!isThreeDigitE164CountryCode(theCountryCode))
            return false;
        break;
    default:
        /* FIXME -- should error or maybe even assert here */
        return false;
    }
    return isValidE164CountryCodeForType(theCountryCode, theType);
}

/*
 * isOneDigitE164CountryCode returns true if theCountryCode is a single digit
 * and false otherwise.
 */
static inline bool isOneDigitE164CountryCode(E164CountryCode theCountryCode)
{
    return ((0 <= theCountryCode) && (9 >= theCountryCode));
}

/*
 * isTwoDigitE164CountryCode returns true if theCountryCode is two digits
 * and false otherwise.
 */
static inline bool isTwoDigitE164CountryCode(E164CountryCode theCountryCode)
{
    return ((10 <= theCountryCode) && (99 >= theCountryCode));
}

/*
 * isThreeDigitE164CountryCode returns true if theCountryCode is three digits
 * and false otherwise.
 */
static inline bool isThreeDigitE164CountryCode(E164CountryCode theCountryCode)
{
    return ((100 <= theCountryCode) && (999 >= theCountryCode));
}

/*
 * isUnassignedE164Type returns true if aType is unassigned or false otherwise.
 */
static inline bool isUnassignedE164Type(E164Type aType)
{
    return ((E164SpareWithoutNote == aType) || (E164SpareWithNote == aType) || (E164Reserved == aType));
}

/*
 * isInvalidE164Type returns true if aType is invalid or false otherwise.
 */
static inline bool isInvalidE164Type(E164Type aType)
{
    return (E164Invalid == aType);
}

static inline E164Type e164TypeForCountryCode(E164CountryCode theCountryCode)
{
    checkE164CountryCodeForRangeError(theCountryCode);
    return e164TypeFor[theCountryCode];
}

#endif /* !E164_BASE_C */
