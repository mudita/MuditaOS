#include "FilesystemEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto FilesystemEndpoint::handle(Context &context) -> void
{
    LOG_DEBUG("handle");
    switch (context.getMethod())
    {
        case http::Method::post:
            run(context);
            break;
        default:
            break;
    }
}
static bool isWritable(const fs::path file)
{
    vfs::FILE *fp = vfs.fopen(file.c_str(), "w");
    if (fp != nullptr) {
        vfs.fclose(fp);
        return true;
    }
    else {
        return false;
    }
}

auto FilesystemEndpoint::run(Context &context) -> sys::ReturnCodes
{
    LOG_DEBUG("running");
    sys::ReturnCodes returnCode = sys::ReturnCodes::Failure;
    std::string cmd = context.getBody()[parserFSM::json::filesystem::command].string_value();

    context.setResponseBody(json11::Json::object( {{json::status, std::to_string(static_cast<int>(sys::ReturnCodes::Failure))}}));

    ServiceDesktop *owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
    if (owner) {
        if (cmd == parserFSM::json::filesystem::commands::download) {
            fs::path filePath = context.getBody()[parserFSM::json::fileName].string_value();
            // fs::path tmpFilePath = vfs.getTempPath(filePath);
            fs::path tmpFilePath = purefs::dir::os_updates / filePath;

            uint32_t fileSize = context.getBody()[parserFSM::json::fileSize].int_value();

            LOG_DEBUG("got owner, file %s", tmpFilePath.c_str());

            if (isWritable(tmpFilePath)) {
                LOG_INFO("download %lu bytes to: %s", fileSize, tmpFilePath.c_str());

                if (owner->startDownload(tmpFilePath, fileSize) == sys::ReturnCodes::Success) {
                    context.setResponseBody(json11::Json::object(
                        {{json::status, std::to_string(static_cast<int>(sys::ReturnCodes::Success)) },
                            {json::uuid, std::to_string(context.getUuid())}}
                        ));

                    returnCode = sys::ReturnCodes::Success;
                }

            } else {
                LOG_ERROR("download command failed, can't write %lu bytes to: %s", fileSize, tmpFilePath.c_str());
            }

        } else {
            LOG_ERROR("unknown command: %s", cmd.c_str());
        }
    }

    MessageHandler::putToSendQueue(context.createSimpleResponse());
    return returnCode;
}