#include "EndpointHandler.hpp"
#include "ParserUtils.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include <common_data/EventStore.hpp>
#include <string>
#include <sstream>
#include <../module-utils/microtar/src/microtar.h>

bool endsWith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

struct FSEntry
{
    FSEntry(int _attrs, uint32_t _size, std::string _name, int _children=0)
        : attrs(_attrs), size(_size), name(_name), children(_children) {}
    int attrs;
    uint32_t size;
    std::string name;
    int children;

    json11::Json to_json() const
    {
        return (json11::Json::object
        {
            {"attrs", attrs},
            {"size", std::to_string(size)},
            {"name", name},
            {"children", std::to_string(children)}
        });
    }
};

sys::ReturnCodes EndpointHandler::storage(uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService)
{
    if (httpMethod == static_cast<uint8_t>(parserutils::http::Method::get))
    {
        std::stringstream pathStream;
        pathStream << "/sys/";
        pathStream << body["dirName"].string_value();

        std::vector<vfs::DirectoryEntry> entries = vfs.listdir(pathStream.str().c_str());
        std::vector<FSEntry> fsEntries;
        for (auto entry : entries)
        {
            if (endsWith (entry.fileName, ".tar"))
            {
                mtar_t tar;
                mtar_header_t h;

                std::stringstream tarFileName;
                tarFileName << "/sys/";
                tarFileName << entry.fileName;

                mtar_open(&tar, tarFileName.str().c_str(), "r");
                int children = 0;
                LOG_INFO("TAR_FILE: %s", tarFileName.str().c_str());
                while ( (mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD )
                {
                    LOG_INFO("   %d/%s (%d bytes)", children, h.name, h.size);
                    mtar_next(&tar);
                    children++;
                }

                fsEntries.push_back ( {(int)entry.attributes, entry.fileSize, entry.fileName, children} );
            }
            else
            {
                fsEntries.push_back({(int)entry.attributes, entry.fileSize, entry.fileName});
            }
        }

        json11::Json responseBodyJson = json11::Json ({ {"fsEntries", fsEntries}, {"uuid", std::to_string(uuid)} });

        int statusCode = static_cast<int>(parserutils::http::Code::OK);
        int endpoint = static_cast<int>(parserutils::Endpoint::storage);

        json11::Json responsePayloadJson = json11::Json::object({
                                                                    {parserutils::json::endpoint, endpoint},
                                                                    {parserutils::json::status, statusCode},
                                                                    {parserutils::json::uuid, std::to_string(uuid)},
                                                                    {parserutils::json::body, responseBodyJson}
                                                                });

        responseStr = EndpointHandler::buildResponseStr(responsePayloadJson.dump().size(), responsePayloadJson.dump());

        LOG_INFO ("storage debug: %s", responseStr.c_str());
        return sys::ReturnCodes::Success;
    }
    else
    {
        LOG_ERROR("Incorrect method");
        return sys::ReturnCodes::Failure;
    }
}//
// Created by atom on 3/3/20.
//
