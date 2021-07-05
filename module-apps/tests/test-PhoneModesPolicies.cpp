// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "notifications/policies/CallNotificationPolicy.hpp"
#include "notifications/policies/SMSNotificationPolicy.hpp"
#include "notifications/policies/NotificationsListPolicy.hpp"

using namespace notifications;

TEST_CASE("Connected Mode notifications - calls policy test")
{
    CallNotificationPolicy callPolicy;

    callPolicy.updateCurrentCall(sys::phone_modes::PhoneMode::Connected);
    REQUIRE(callPolicy.isPopupAllowed());
    REQUIRE(callPolicy.isRingtoneAllowed());
    REQUIRE(!callPolicy.isNumberCheckRequired());
}

TEST_CASE("Connected Mode notifications - sms policy test")
{
    SMSNotificationPolicy callPolicy;

    callPolicy.updateCurrentSMS(sys::phone_modes::PhoneMode::Connected);
    REQUIRE(callPolicy.isRingtoneAllowed());
}

TEST_CASE("Connected Mode notifications - list policy test")
{
    NotificationsListPolicy listPolicy;

    auto phoneLocked             = true;
    auto notificationsWhenLocked = true;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::Connected, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());

    phoneLocked             = true;
    notificationsWhenLocked = false;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::Connected, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());

    phoneLocked             = false;
    notificationsWhenLocked = false;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::Connected, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());

    phoneLocked             = false;
    notificationsWhenLocked = true;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::Connected, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());
}

TEST_CASE("DoNotDisturb Mode notifications  - calls policy test")
{
    CallNotificationPolicy callPolicy;

    callPolicy.updateCurrentCall(sys::phone_modes::PhoneMode::DoNotDisturb);
    REQUIRE(!callPolicy.isPopupAllowed());
    REQUIRE(!callPolicy.isRingtoneAllowed());
    REQUIRE(callPolicy.isNumberCheckRequired());
    REQUIRE(callPolicy.isDismissedCallNotificationAllowed());

    SECTION("Number in/not in Favourites")
    {
        auto numberInFavourites   = true;
        auto callsFromFavsAllowed = true;
        callPolicy.numberCheck(callsFromFavsAllowed, numberInFavourites);
        REQUIRE(callPolicy.isPopupAllowed());
        REQUIRE(callPolicy.isRingtoneAllowed());

        numberInFavourites   = true;
        callsFromFavsAllowed = false;
        callPolicy.numberCheck(callsFromFavsAllowed, numberInFavourites);
        REQUIRE(!callPolicy.isPopupAllowed());
        REQUIRE(!callPolicy.isRingtoneAllowed());

        numberInFavourites   = false;
        callsFromFavsAllowed = true;
        callPolicy.numberCheck(callsFromFavsAllowed, numberInFavourites);
        REQUIRE(!callPolicy.isPopupAllowed());
        REQUIRE(!callPolicy.isRingtoneAllowed());

        numberInFavourites   = false;
        callsFromFavsAllowed = false;
        callPolicy.numberCheck(callsFromFavsAllowed, numberInFavourites);
        REQUIRE(!callPolicy.isPopupAllowed());
        REQUIRE(!callPolicy.isRingtoneAllowed());
    }
}

TEST_CASE("DoNotDisturb Mode notifications - sms policy test")
{
    SMSNotificationPolicy callPolicy;

    callPolicy.updateCurrentSMS(sys::phone_modes::PhoneMode::DoNotDisturb);
    REQUIRE(!callPolicy.isRingtoneAllowed());
}

TEST_CASE("DoNotDisturb Mode notifications  - list policy test")
{
    NotificationsListPolicy listPolicy;

    auto phoneLocked             = true;
    auto notificationsWhenLocked = true;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::DoNotDisturb, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());

    phoneLocked             = true;
    notificationsWhenLocked = false;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::DoNotDisturb, phoneLocked, notificationsWhenLocked);
    REQUIRE(!listPolicy.updateListAllowed());
    REQUIRE(!listPolicy.showListWhenLocked());

    phoneLocked             = false;
    notificationsWhenLocked = false;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::DoNotDisturb, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(!listPolicy.showListWhenLocked());

    phoneLocked             = false;
    notificationsWhenLocked = true;
    listPolicy.updateCurrentList(sys::phone_modes::PhoneMode::DoNotDisturb, phoneLocked, notificationsWhenLocked);
    REQUIRE(listPolicy.updateListAllowed());
    REQUIRE(listPolicy.showListWhenLocked());
}

TEST_CASE("Offline Mode notifications - sms policy test")
{
    SMSNotificationPolicy callPolicy;

    callPolicy.updateCurrentSMS(sys::phone_modes::PhoneMode::Offline);
    REQUIRE(callPolicy.isRingtoneAllowed());
}

TEST_CASE("Offline Mode notifications - calls policy test")
{
    CallNotificationPolicy callPolicy;

    callPolicy.updateCurrentCall(sys::phone_modes::PhoneMode::Offline);
    REQUIRE(!callPolicy.isPopupAllowed());
    REQUIRE(!callPolicy.isRingtoneAllowed());
    REQUIRE(!callPolicy.isNumberCheckRequired());
    REQUIRE(!callPolicy.isDismissedCallNotificationAllowed());
}
