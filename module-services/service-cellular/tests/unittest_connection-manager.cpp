// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <service-cellular/connection-manager/ConnectionManager.hpp>
#include <service-cellular/connection-manager/ConnectionManagerCellularCommandsInterface.hpp>

using namespace testing;

class MockCellular : public ConnectionManagerCellularCommandsInterface
{
  public:
    MOCK_METHOD(bool, disconnectFromNetwork, (), (override));
    MOCK_METHOD(bool, connectToNetwork, (), (override));
    MOCK_METHOD(bool, isConnectedToNetwork, (), (override));
    MOCK_METHOD(bool, clearNetworkIndicator, (), (override));
    MOCK_METHOD(bool, hangUpOngoingCall, (), (override));
    MOCK_METHOD(bool, isConnectionTimerActive, (), (override));
    MOCK_METHOD(void, startConnectionTimer, (), (override));
    MOCK_METHOD(void, stopConnectionTimer, (), (override));
};

TEST(ConnectionManager, onPhoneModeChange)
{
    std::shared_ptr<MockCellular> mock = std::make_shared<MockCellular>();
    ConnectionManager tested(true, std::chrono::minutes{5}, mock);

    // Connected / Do not disturb to Offline
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, hangUpOngoingCall()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, clearNetworkIndicator()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, stopConnectionTimer()).Times(1);

    EXPECT_CALL(*mock, startConnectionTimer()).Times(0);
    EXPECT_CALL(*mock, connectToNetwork()).Times(0);
    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Offline);

    // Connected / Do not disturb to Messages only
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, hangUpOngoingCall()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, clearNetworkIndicator()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, startConnectionTimer()).Times(1);

    EXPECT_CALL(*mock, stopConnectionTimer()).Times(0);
    EXPECT_CALL(*mock, connectToNetwork()).Times(0);
    tested.setFlightMode(false);
    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Offline);

    // Offline to Connected / Do not disturb
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, connectToNetwork()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(0);
    tested.setFlightMode(true);
    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Connected);

    // Messages only to Connected / Do not disturb
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, stopConnectionTimer()).Times(1);
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, connectToNetwork()).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(0);
    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Connected);
}

TEST(ConnectionManager, onTimerTick_Messages_only)
{
    std::shared_ptr<MockCellular> mock = std::make_shared<MockCellular>();
    ConnectionManager tested(false, std::chrono::minutes{15}, mock);

    // Messages only, interval 15 min
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, startConnectionTimer()).Times(1);
    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Offline);

    EXPECT_CALL(*mock, connectToNetwork()).Times(1).WillOnce(Return(true));
    for (auto i = 0; i < 15; i++) {
        tested.onTimerTick();
    }

    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(1).WillOnce(Return(true));
    for (auto i = 0; i < 5; i++) {
        tested.onTimerTick();
    }
}

TEST(ConnectionManager, onTimerTick_Offline)
{
    std::shared_ptr<MockCellular> mock = std::make_shared<MockCellular>();
    ConnectionManager tested(true, std::chrono::minutes{15}, mock);

    // Offline, timer was active
    EXPECT_CALL(*mock, isConnectedToNetwork()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*mock, isConnectionTimerActive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, stopConnectionTimer()).Times(1);

    tested.onPhoneModeChange(sys::phone_modes::PhoneMode::Offline);

    EXPECT_CALL(*mock, connectToNetwork()).Times(0);
    EXPECT_CALL(*mock, disconnectFromNetwork()).Times(0);
    for (auto i = 0; i < 20; i++) {
        tested.onTimerTick();
    }
}
