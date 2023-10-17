// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/SwitchData.hpp>

///  @brief Switch data for PhoneLockedInfoWindow to specify the
///  proper init window stage of unlocking process
class PhoneLockedInfoData : public gui::SwitchData
{
  public:
    /// Enum descriging two possible unlocing stages:\n
    /// Idle stage means user has not started unlocking The device waits for
    /// user to press UNLOCK (Enter) button to start unlocking procedure.\n
    /// Waiting stage occurs when UNLOCK button is pressed and device
    /// waits for some time for the # button to be pressed to complete unlock.
    enum class Stage
    {
        Idle,
        Waiting
    };

    /// Constructor
    /// @param stage stage of unlocking carried in this switch data
    explicit PhoneLockedInfoData(Stage stage) : currentStage(std::move(stage))
    {}

    /// Returns the stage of unlocking carried in this switch data
    /// @return information carried by this switch data
    Stage getStage() const noexcept
    {
        return currentStage;
    }

  private:
    /// Current unlocking stage
    Stage currentStage;
};
