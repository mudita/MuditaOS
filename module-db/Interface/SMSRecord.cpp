
/*
 * @file SMSRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "SMSRecord.hpp"

SMSRecord::SMSRecord():
Record() {

}

SMSRecord::~SMSRecord() {

}

bool SMSRecord::Add() {
    db = std::make_unique<SmsDB>();

    //db->threads.Add()
}

bool SMSRecord::Remove() {

}

bool SMSRecord::Update() {

}