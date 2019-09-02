#pragma once

#include "service-db/api/DBServiceAPI.hpp"
#include <ContactRecord.hpp>

/// helper class to create proper ContactRecord from map of elements and vice versa
class ContactBuilder {
    ContactRecord record;
    std::map<std::string, UTF8> mapping;

  public:
    ContactBuilder();
    ContactBuilder(ContactBuilder &) = default;
    /// fills record with values from params and store params in `map` element
    ContactBuilder(std::map<std::string, UTF8> &params);
    /// fills mapping with values from contact and stores contact in `record` element
    ContactBuilder(ContactRecord contact);
    void load(std::map<std::string, UTF8> &params);
    void load(ContactRecord contact);
    // verifies if ContctRecord stored is complete
    bool verify();
    ContactRecord &getRecord();
    std::map<std::string, UTF8> getMap();
    UTF8& speeddial_str();
};
