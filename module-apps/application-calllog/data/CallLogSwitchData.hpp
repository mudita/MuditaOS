/*
 * @file CallLogSwitchData.h
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 8.10.2019
 * @brief Call Log Switch Data
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "CalllogRecord.hpp"
#include "SwitchData.hpp"

namespace calllog {

const inline std::string CALLLOG_SWITCH_DATA_STR = "CallLogSwitchData";

class CallLogSwitchData: public gui::SwitchData {
protected:
	CalllogRecord record;
public:
	CallLogSwitchData() = delete; 
	CallLogSwitchData( CalllogRecord record ) : SwitchData(CALLLOG_SWITCH_DATA_STR), record{ record } {};
	virtual ~CallLogSwitchData() = default;

	const CalllogRecord& getRecord() const { return record; };
};

} // namespace calllog 
