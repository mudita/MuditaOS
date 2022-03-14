// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>
#include <Utils.hpp> //for magic_enum

namespace at
{

    static const int MAX_AT_ERROR_VALUE = 1000; // use for magic_enum limitations, see end of this file

    /** @enum at::EquipmentErrorCode
     *  @brief is a strongly typed enum class for Equipment (CME) AT commands error.
     *  Base on Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
     *  @var at::EquipmentErrorCode::SIM_PIN_required
     *  for function like CPIN return information about PIN required
     */
    enum class EquipmentErrorCode : uint32_t
    {
        PhoneFailure                  = 0,
        NoConnection                  = 1,
        PhoneAdaptorLinkReserved      = 2,
        OperationNotAllowed           = 3,
        OperationNotAllowed1          = 4, /*!<  not sure about meaning, different msg from modem */
        PH_SIM_PINRequired            = 5,
        PH_FSIM_PINRequired           = 6,
        PH_FSIM_PUKRequired           = 7,
        SIMNotInserted                = 10,
        SIM_PIN_required              = 11,
        SIM_PUKRequired               = 12,
        SIMFailure                    = 13,
        SIMBusy                       = 14, /*!< could be returned on removed card and QPINC, PINC ret 10 */
        SIMWrong                      = 15,
        IncorrectPassword             = 16,
        SIM_PIN2Required              = 17,
        SIM_PUK2Required              = 18,
        MemoryFull                    = 20,
        InvalidIndex                  = 21,
        NotFound                      = 22,
        MemoryFailure                 = 23,
        TextStringTooLong             = 24,
        InvalidCharactersInTextString = 25,
        DialStringTooLong             = 26,
        InvalidCharactersInDialString = 27,
        NoNetworkService              = 30,
        NetworkTimeout                = 31,
        NetworkNotAllowed_EC          = 32, /*!< emergency calls only */
        NetworkPersPINRequired        = 40,
        NetworkPersPUKRequired        = 41,
        NetworkSubsetPersPINRequired  = 42,
        NetworkSubsetPersPUKRequired  = 43,
        ServiceProvPersPINRequired    = 44,
        ServiceProvPersPUKRequired    = 45,
        CorporatePersPINRequired      = 46,
        CorporatePersPUKRequired      = 47,
        PhoneBusy                     = 258,
        AudioUnknownError             = 901,
        AudioInvalidParameters        = 902,
        AudioOperationNotSupported    = 903,
        AudioDeviceBusy               = 904,

        Unknown       = MAX_AT_ERROR_VALUE + 1, /*!< for error number out of the list */
        NoInformation = MAX_AT_ERROR_VALUE + 2  /*!< in case of ERROR (not+CME), could be call AT+CEER */
    };

    /** @enum at::NetworkErrorCode
     *  @brief is a strongly typed enum class for Network (CMS) AT commands error.
     *  Base on Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
     */
    enum class NetworkErrorCode : uint32_t
    {
        MEFailure                   = 300,
        SMS_MEReserved              = 301,
        OperationNotAllowed         = 302,
        OperationNotSupported       = 303,
        InvalidPDUMode              = 304,
        InvalidTextMode             = 305,
        SIMNotInserted              = 310,
        SIMPinNecessary             = 311,
        PH_SIMPinNecessary          = 312,
        SIMFailure                  = 313,
        SIMBusy                     = 314,
        SIMWrong                    = 315,
        SIM_PUKRequired             = 316,
        SIM_PIN2Required            = 317,
        SIM_PUK2Required            = 318,
        MemoryFailure               = 320,
        InvalidMemoryIndex          = 321,
        MemoryFull                  = 322,
        SMSCAddressUnknown          = 330,
        NoNetwork                   = 331,
        NetworkTimeout              = 332,
        ModemUnknown                = 500, /*!< this one i other then "Unknown" this Unknown id defined by quectel */
        SIMNotReady                 = 512,
        MessageLengthExceeds        = 513,
        InvalidRequestParameters    = 514,
        MEStorageFailure            = 515,
        InvalidServiceMode          = 517,
        MoreMessageToSendStateError = 528,
        MO_SMSIsNotAllow            = 529,
        GPRSIsSuspended             = 530,
        MEStorageFull               = 531,

        Unknown       = MAX_AT_ERROR_VALUE + 1, /*!< for error number out of the list */
        NoInformation = MAX_AT_ERROR_VALUE + 2  /*!< in case of ERROR (not+CMS) */
    };

} // namespace at

/** This part of code is extension for magic_enum class due to documentation,
 * below we extended AT error enum class, based on const MAX_AT_ERROR_VALUE
 * which should have value higher then maximum AT error value but lower then
 * magic num library limitations which is INT16_MAX-2 (-2 for Unknown and NoInformation)
 */
namespace magic_enum
{
    template <> struct enum_range<at::NetworkErrorCode>
    {
        static constexpr int min = 0;
        static constexpr int max = at::MAX_AT_ERROR_VALUE + 2;
    };

    template <> struct enum_range<at::EquipmentErrorCode>
    {
        static constexpr int min = 0;
        static constexpr int max = at::MAX_AT_ERROR_VALUE + 2;
    };

} // namespace magic_enum
