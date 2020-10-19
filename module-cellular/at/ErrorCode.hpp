#pragma once
// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <limits>
#include <Utils.hpp> //for magic_enum
//#include <magic_enum.hpp>

namespace at
{
    /*
     *
     * Base on Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
Code of <err> Meaning
0 Phone failure
1 No connection to phone
2 Phone-adaptor link reserved
3 Operation not allowed
4 Operation not supported
5 PH-SIM PIN required
6 PH-FSIM PIN required
7 PH-FSIM PUK required
10 SIM not inserted
11 SIM PIN required
12 SIM PUK required
13 SIM failure
14 SIM busy
15 SIM wrong
16 Incorrect password
17 SIM PIN2 required
18 SIM PUK2 required
20 Memory full
21 Invalid index
22 Not found
23 Memory failure
24 Text string too long
25 Invalid characters in text string
26 Dial string too long
27 Invalid characters in dial string
30 No network service
31 Network timeout
32 Network not allowed - emergency calls only
40 Network personalization PIN required
41 Network personalization PUK required
42 Network subset personalization PIN required
43 Network subset personalization PUK required
44 Service provider personalization PIN required
45 Service provider personalization PUK required
46 Corporate personalization PIN required
47 Corporate personalization PUK required
901 Audio unknown error
902 Audio invalid parameters
903 Audio operation not supported
904 Audio device busy

//----------------------- second list CMS
300 ME failure
301 SMS ME reserved
302 Operation not allowed
303 Operation not supported
304 Invalid PDU mode
305 Invalid text mode
310 SIM not inserted
311 SIM pin necessary
312 PH SIM pin necessary
313 SIM failure
314 SIM busy
315 SIM wrong
316 SIM PUK required
317 SIM PIN2 required
318 SIM PUK2 required
320 Memory failure
321 Invalid memory index
322 Memory full
330 SMSC address unknown
331 No network
332 Network timeout
500 Unknown
512 SIM not ready
513 Message length exceeds
514 Invalid request parameters
515 ME storage failure
517 Invalid service mode
528 More message to send state error
529 MO SMS is not allow
530 GPRS is suspended
531 ME storage full
     */

    static const int MAX_AT_ERROR_VALUE = 1000;//use for magic_enum limitations, see end of this file

    /** @enum at::EquipmentErrorCode
     *  @brief is a strongly typed enum class for Equipment (CME) AT commands error.
     *  Base on Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
     *  @var at::EquipmentErrorCode::SIM_PIN_required
     *  for function like CPIN return information about PIN required
     */
    enum class EquipmentErrorCode : uint32_t
    {
        PhoneFailure             = 0,
        NoConnection             = 1,
        PhoneAdaptorLinkReserved = 2,
        OperationNotAllowed      = 3,
        OperationNotAllowed1     = 4, // not sure about meaning, different msg from modem
        PH_SIM_PINRequired       = 5,
        PH_FSIM_PINRequired      = 6,
        PH_FSIM_PUKRequired      = 7,
        SIMNotInserted           = 10,
        SIM_PIN_required         = 11,
        SIM_PUKRequired          = 12,
        SIMFailure               = 13,
        SIMBusy                  = 14,
        SIMWrong                 = 15,
        IncorrectPassword        = 16,
        SIM_PIN2Required         = 17,
        SIM_PUK2Required         = 18,
        MemoryFull               = 20,
        InvalidIndex = 21,
        NotFound = 22,
        MemoryFailure = 23,
        TextStringTooLong = 24,
        InvalidCharactersInTextString = 25,
        DialStringTooLong = 26,
        InvalidCharactersInDialString = 27,
        NoNetworkService = 30,
        NetworkTimeout = 31,
        NetworkNotAllowed_EC = 32,// - emergency calls only
        NetworkPersPINRequired = 40,
        NetworkPersPUKRequired = 41,
        NetworkSubsetPersPINRequired = 42,
        NetworkSubsetPersPUKRequired = 43,
        ServiceProvPersPINRequired = 44,
        ServiceProvPersPUKRequired = 45,
        CorporatePersPINRequired = 46,
        CorporatePersPUKRequired = 47,
        AudioUnknownError = 901,
        AudioInvalidParameters = 902,
        AudioOperationNotSupported = 903,
        AudioDeviceBusy = 904,

        Unknown                  = MAX_AT_ERROR_VALUE+1, // for error number out of the list
        NoInformation            = MAX_AT_ERROR_VALUE+2  // in case of ERROR (not+CME), could be call AT+CEER
    };


    /** @enum at::NetworkErrorCode
     *  @brief is a strongly typed enum class for Network (CMS) AT commands error.
     *  Base on Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf
     */
    enum class NetworkErrorCode : uint32_t
    {

        MEFailure = 300,
        SMS_MEReserved = 301,
        OperationNotAllowed = 302,
        OperationNotSupported = 303,
        InvalidPDUMode = 304,
        InvalidTextMode = 305,
        SIMNotInserted = 310,
        SIMPinNecessary = 311,
        PH_SIMPinNecessary = 312,
        SIMFailure = 313,
        SIMBusy = 314,
        SIMWrong = 315,
        SIM_PUKRequired = 316,
        SIM_PIN2Required = 317,
        SIM_PUK2Required = 318,
        MemoryFailure = 320,
        InvalidMemoryIndex = 321,
        MemoryFull = 322,
        SMSCAddressUnknown = 330,
        NoNetwork = 331,
        NetworkTimeout = 332,
        ModemUnknown = 500, //this one i other then "Unknown" this Unknown id defined by quectel
        SIMNotReady = 512,
        MessageLengthExceeds = 513,
        InvalidRequestParameters = 514,
        MEStorageFailure = 515,
        InvalidServiceMode = 517,
        MoreMessageToSendStateError = 528,
        MO_SMSIsNotAllow = 529,
        GPRSIsSuspended = 530,
        MEStorageFull = 531,

        Unknown                  = MAX_AT_ERROR_VALUE+1, // for error number out of the list
        NoInformation            = MAX_AT_ERROR_VALUE+2  // in case of ERROR (not+CMS)
    };

    } // namespace at




/*This part of code is extension for magic_enum class due to documentation,
 * below we extended AT error enum class, based on const MAX_AT_ERROR_VALUE
 * which should have value higher then maximum AT error value but lower then
 * magic num library limitations which is INT16_MAX-2 (-2 for Unknown and NoInformation)
 */

namespace magic_enum {
    template <>
    struct enum_range<at::NetworkErrorCode> {
        static constexpr int min = 0;
        static constexpr int max = at::MAX_AT_ERROR_VALUE+2;
    };


    template <>
    struct enum_range<at::EquipmentErrorCode> {
        static constexpr int min = 0;
        static constexpr int max = at::MAX_AT_ERROR_VALUE+2;
    };

}