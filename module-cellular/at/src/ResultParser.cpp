// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ResultParser.hpp"

#include <Utils.hpp>
#include <ResultQeccnum.hpp>

using namespace at;

std::shared_ptr<at::Result> ResultParser::parse()
{
    auto responseContains = [this](const std::string &prefix) {
        for (auto &responsePart : response) {
            auto ret = responsePart.find(prefix);
            if (ret != std::string::npos) {
                return true;
            }
        }
        return false;
    };

    if (statusCode != at::Result::StatusCode::OK) {
        return std::make_shared<Result>(response, statusCode, errorCode);
    }

    if (responseContains(std::string(ResultQueccnum::prefix))) {
        return std::make_shared<at::ResultQueccnum>(response, statusCode, errorCode);
    }

    return std::make_shared<Result>(response, statusCode, errorCode);
}

bool ResultParser::appendPartialResult(const std::vector<std::string> &part)
{
    response.insert(std::end(response), std::begin(part), std::end(part));
    return checkError(part);
}

bool ResultParser::checkError(const std::vector<std::string> &partialResult)
{
    if (checkCmxError(CmxErrorType::CmeError, partialResult)) {
        return true;
    }
    if (checkCmxError(CmxErrorType::CmsError, partialResult)) {
        return true;
    }
    if (checkAtError(partialResult)) {
        return true;
    }
    return false;
}

void ResultParser::setStatusCode(at::Result::StatusCode code)
{
    statusCode = code;
}

const std::vector<std::string> &at::ResultParser::getResponse()
{
    return response;
}

bool ResultParser::checkAtError(const std::vector<std::string> &partialResult)
{
    if (partialResult.size()) {
        for (auto &el : partialResult) {
            if (el.compare(0, std::string(STATUS_OK).length(), std::string(STATUS_OK)) == 0) {
                setStatusCode(at::Result::StatusCode::OK);
            }
            else if (el.compare(0, std::string(STATUS_ERROR).length(), std::string(STATUS_ERROR)) == 0) {
                setStatusCode(at::Result::StatusCode::ERROR);
                return true;
            }
        }
    }
    return false;
}

bool ResultParser::checkCmxError(CmxErrorType errorType, const std::vector<std::string> &partialResult)
{
    std::string cmxErrorStr(errorType == CmxErrorType::CmeError ? CME_ERROR : CMS_ERROR);

    if (partialResult.size()) {
        for (auto &cmxerr : partialResult) {
            if (cmxerr.compare(0, cmxErrorStr.length(), cmxErrorStr) == 0) {
                auto serr = utils::trim(cmxerr.substr(cmxErrorStr.length(), cmxerr.length() - cmxErrorStr.length()));
                cmxError  = utils::getNumericValue<uint32_t>(serr);
            }
        }
    }

    if (cmxError) {
        setStatusCode(at::Result::StatusCode::ERROR);
        if (errorType == CmxErrorType::CmeError) {
            auto tmp_ec = magic_enum::enum_cast<EquipmentErrorCode>(cmxError);
            if (tmp_ec.has_value()) {
                LOG_ERROR("%s", utils::enumToString(tmp_ec.value()).c_str());
                errorCode = tmp_ec.value();
                return true;
            }
        }
        else if (errorType == CmxErrorType::CmsError) {
            auto tmp_ec = magic_enum::enum_cast<NetworkErrorCode>(cmxError);
            if (tmp_ec.has_value()) {
                LOG_ERROR("%s", utils::enumToString(tmp_ec.value()).c_str());
                errorCode = tmp_ec.value();
                return true;
            }
        }
        LOG_ERROR("Unknow CME error code %" PRIu32, cmxError);
        errorCode = at::EquipmentErrorCode::Unknown;
        return true;
    }

    return false;
}
