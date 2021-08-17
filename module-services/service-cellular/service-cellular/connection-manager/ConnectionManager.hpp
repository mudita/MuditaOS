// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ConnectionManagerCellularCommandsInterface.hpp"
#include "module-services/service-cellular/service-cellular/ServiceCellular.hpp"
#include <module-sys/PhoneModes/Observer.hpp>
#include <functional>

// connection timer period is 1 minute, connect to network for 5 minutes
constexpr std::chrono::minutes connectedPeriod{5};

/**
 * @brief Intended for GSM network connection management depending on the selected phone mode.
 * Switching modes takes place at Phone Modes Observer event.
 */
class ConnectionManager
{
  public:
    /**
     * @brief Constructor
     * @param flightMode current setting for flight mode
     * @param interval current setting for connection interval in Message only mode
     * @param serviceCellular pointer to service cellular methods provider
     */
    explicit ConnectionManager(const bool flightMode,
                               const std::chrono::minutes interval,
                               std::shared_ptr<ConnectionManagerCellularCommandsInterface> serviceCellular)
        : isFlightMode(flightMode), connectionInterval(interval), cellular(serviceCellular)
    {}
    /**
     * @brief Handles Phone mode change. Called on event provided by Phone mode observer or settinhs change.
     * @param mode current Phone mode
     * @return true on success, false on fail
     */
    auto onPhoneModeChange(sys::phone_modes::PhoneMode mode) -> bool;
    /**
     * @brief Sets interval of connecting to the GSM network in Offline Message only mode
     * @param interval connection interval in minutes
     */
    void setInterval(const std::chrono::minutes interval);
    /**
     * @biref Sets flight mode.
     * @param mode flight mode enabled/disabled
     */
    void setFlightMode(const bool mode);
    /**
     * @brief Handles cyclic connecting/disconnecting from the GSM network in Offline Message only mode. Called in
     * service timer handler.
     */
    void onTimerTick();

    /// Should we always dismiss incoming calls?
    /// @return true or false depending on state of forceDismissCallsMode flag
    /// @see forceDismissCallsMode
    auto forceDismissCalls() -> bool;

  private:
    bool isFlightMode;
    std::chrono::minutes connectionInterval{0};
    std::chrono::minutes minutesOfflineElapsed{0};
    std::chrono::minutes minutesOnlineElapsed{0};
    std::shared_ptr<ConnectionManagerCellularCommandsInterface> cellular;
    bool onlinePeriod = false;

    /// Flag determining if we should always dismiss incoming calls - even when
    /// we are in offline mode (messages only) and we connect to network to poll
    /// for new messages
    bool forceDismissCallsFlag = false;

    /**
     * @brief Checks if flightMode and connection interval are set as Messages only mode
     * @return true when Messages only is configured, false when not
     */
    auto isMessagesOnlyMode() -> bool;
    /**
     * It's handling change Phone mode to common offline modes
     * @return true on success, false on fail
     */
    auto handleModeChangeToCommonOffline() -> bool;
    /**
     * It's handling change Phone mode to Offline mode
     * @return true on success, false on fail
     */
    void handleModeChangeToOfflineMode();
    /**
     * It's handling change Phone mode to Messages only mode
     * @return true on success, false on fail
     */
    void handleModeChangeToMessageOnlyMode();
    /**
     * It's handling change Phone mode to Connected / Do not disturb mode
     * @return true on success, false on fail
     */
    auto handleModeChangeToConnected() -> bool;
};
