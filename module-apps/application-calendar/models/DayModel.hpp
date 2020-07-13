#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <map>
#include <vector>
#include <string>

class DayModel
{
  public:
    std::string number;
    const uint32_t x;
    const uint32_t y;

  public:
    DayModel(std::string number, const uint32_t &x, const uint32_t &y);
    virtual ~DayModel() = default;
};
