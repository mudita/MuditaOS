
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

    auto smsdb = std::make_unique<SmsDB>();
    auto contactdb = std::make_unique<ContactsDB>();


    // Check if contact exists
    auto contact = contactdb->number.GetByName(number.c_str());
    if(contact.ID == 0){


    }


    //db->threads.Add()
}

bool SMSRecord::Remove() {

}

bool SMSRecord::Update() {

}