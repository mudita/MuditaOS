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
    std::string number = "";
    uint32_t x = 0;
    uint32_t y = 0;
    
    public:
    DayModel(const char *number,const uint32_t &x,const uint32_t &y);
    virtual ~DayModel() = default;
};