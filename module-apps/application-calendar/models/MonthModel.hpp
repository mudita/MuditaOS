#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <map>
#include <string>
#include "DayModel.hpp"
#include <vector>
#include "DatabaseModel.hpp"

class MonthModel
{  
    public:
    std::string name = "";
    std::vector< std::unique_ptr<DayModel> > days;
    uint32_t numberOfDays = 0;
    
    public:
    MonthModel(const char *name,std::vector< std::unique_ptr<DayModel> > &days);
    MonthModel() = default;
    virtual ~MonthModel() = default;

};