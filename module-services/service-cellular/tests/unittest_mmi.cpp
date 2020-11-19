// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <service-cellular/RequestFactory.hpp>
#include <service-cellular/requests/CallForwardingRequest.hpp>
#include <service-cellular/requests/PasswordRegistrationRequest.hpp>
#include <service-cellular/requests/PinChangeRequest.hpp>
#include <service-cellular/requests/ImeiRequest.hpp>
#include <service-cellular/requests/UssdRequest.hpp>

using namespace cellular;

TEST_CASE("MMI requests")
{
    struct TestCase
    {
        const std::string requestString;
        const std::string expectedCommandString;
        const std::type_info &expectedType;
        const bool expectedValid = true;
    };

    std::vector<TestCase> testCases = {
        /// IMEIRequest
        {R"(*#06#)", R"(AT+GSN)", typeid(ImeiRequest)},

        /// CallForwardingRequest
        // all diversions
        {R"(##002#)", R"(AT+CCFC=4,4)", typeid(CallForwardingRequest)},
        // all conditional redirections
        {R"(**004*666555444#)", R"(AT+CCFC=5,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(##004#)", R"(AT+CCFC=5,4)", typeid(CallForwardingRequest)},
        // unconditional divert
        {R"(**21*666555444#)", R"(AT+CCFC=0,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*21#)", R"(AT+CCFC=0,1)", typeid(CallForwardingRequest)},
        {R"(#21#)", R"(AT+CCFC=0,0)", typeid(CallForwardingRequest)},
        {R"(##21#)", R"(AT+CCFC=0,4)", typeid(CallForwardingRequest)},
        {R"(*#21#)", R"(AT+CCFC=0,2)", typeid(CallForwardingRequest)},
        // divert when not answered
        {R"(**61*666555444#)", R"(AT+CCFC=2,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*61#)", R"(AT+CCFC=2,1)", typeid(CallForwardingRequest)},
        {R"(#61#)", R"(AT+CCFC=2,0)", typeid(CallForwardingRequest)},
        {R"(##61#)", R"(AT+CCFC=2,4)", typeid(CallForwardingRequest)},
        {R"(*#61#)", R"(AT+CCFC=2,2)", typeid(CallForwardingRequest)},
        // divert when off or not
        {R"(**62*666555444#)", R"(AT+CCFC=3,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*62#)", R"(AT+CCFC=3,1)", typeid(CallForwardingRequest)},
        {R"(#62#)", R"(AT+CCFC=3,0)", typeid(CallForwardingRequest)},
        {R"(##62#)", R"(AT+CCFC=3,4)", typeid(CallForwardingRequest)},
        {R"(*#62#)", R"(AT+CCFC=3,2)", typeid(CallForwardingRequest)},
        // divert when busy or pressing reject
        {R"(**67*666555444#)", R"(AT+CCFC=1,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*67#)", R"(AT+CCFC=1,1)", typeid(CallForwardingRequest)},
        {R"(#67#)", R"(AT+CCFC=1,0)", typeid(CallForwardingRequest)},
        {R"(##67#)", R"(AT+CCFC=1,4)", typeid(CallForwardingRequest)},
        {R"(*#67#)", R"(AT+CCFC=1,2)", typeid(CallForwardingRequest)},
        // alternative register and on
        {R"(*21*666555444#)", R"(AT+CCFC=0,1,"666555444")", typeid(CallForwardingRequest)},

        /// PasswordRegistrationRequest
        // total incoming and outgoing service barring (empty string)
        {R"(**03**23*111*111#)", R"(AT+CPWD="AB","23","111")", typeid(PasswordRegistrationRequest)},
        // outgoing call barring
        {R"(**03*33*1234*4321*4321#)", R"(AT+CPWD="AO","1234","4321")", typeid(PasswordRegistrationRequest)},
        // outgoing international call barring
        {R"(**03*331*1234*4321*4321#)", R"(AT+CPWD="OI","1234","4321")", typeid(PasswordRegistrationRequest)},
        // outgoing international call barring, excluding to home
        {R"(**03*332*1234*4321*4321#)", R"(AT+CPWD="OX","1234","4321")", typeid(PasswordRegistrationRequest)},
        // incoming call barring
        {R"(**03*35*1234*4321*4321#)", R"(AT+CPWD="AI","1234","4321")", typeid(PasswordRegistrationRequest)},
        // incoming call barring, when international roaming
        {R"(**03*351*1234*4321*4321#)", R"(AT+CPWD="IR","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total incoming and outgoing service barring
        {R"(**03*330*1234*4321*4321#)", R"(AT+CPWD="AB","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total outgoing service barring
        {R"(**03*333*1234*4321*4321#)", R"(AT+CPWD="AG","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total incoming service barring
        {R"(**03*353*1234*4321*4321#)", R"(AT+CPWD="AC","1234","4321")", typeid(PasswordRegistrationRequest)},
        // alternative procedure type *
        {R"(*03*353*1234*4321*4321#)", R"(AT+CPWD="AC","1234","4321")", typeid(PasswordRegistrationRequest)},
        // bad procedure type * fallback to UUSD
        {R"(*#03*353*1234*4321*4321#)", R"(AT+CUSD=1,*#03*353*1234*4321*4321#,15)", typeid(UssdRequest)},
        // bad procedure type ##
        {R"(##03*353*1234*4321*4321#)", std::string(), typeid(CallRequest)},
        // bad procedure type #
        {R"(#03*353*1234*4321*4321#)", std::string(), typeid(CallRequest)},
        // no password
        {R"(**03*353***#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353*24**#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353**34*#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353***34#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // password does not match
        {R"(**03*353*56*46*74#)", std::string(), typeid(PasswordRegistrationRequest), false},

        /// PinChangeRequest
        // Change PIN
        {R"(**04*0000*1111*1111#)", R"(AT+CPWD="SC","0000","1111")", typeid(PinChangeRequest)},
        // Change PIN2
        {R"(**042*0000*1111*1111#)", R"(AT+CPWD="P2","0000","1111")", typeid(PinChangeRequest)},
        // Change PIN by PUK
        {R"(**05*0000*1111*1111#)", R"(AT+CPIN="0000","1111")", typeid(PinChangeRequest)},
        // Change PIN by PUK more than 4
        {R"(**042*00002*11113*11113#)", R"(AT+CPWD="P2","00002","11113")", typeid(PinChangeRequest)},
        // bad procedure type *
        {R"(*042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // bad procedure type ##
        {R"(##042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // bad procedure type #
        {R"(#042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // bad procedure type *#
        {R"(*#042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // no password
        {R"(**042*00002**#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042***11112#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042**11112*#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042**11112*#)", std::string(), typeid(PinChangeRequest), false},
        //  password does not match
        {R"(**042*0000*1111*2222#)", std::string(), typeid(PinChangeRequest), false},
    };

    for (auto &testCase : testCases) {
        RequestFactory requestFactory(testCase.requestString);
        auto request        = requestFactory.create();
        auto requestCommand = request->command();
        INFO("Failed on testcase: " << testCase.requestString);
        REQUIRE(typeid(*request.get()).name() == testCase.expectedType.name());
        REQUIRE(request->isValid() == testCase.expectedValid);
        if (typeid(*request.get()).name() != typeid(CallRequest).name() && request->isValid()) {
            REQUIRE(requestCommand == testCase.expectedCommandString);
        }
    }
}
