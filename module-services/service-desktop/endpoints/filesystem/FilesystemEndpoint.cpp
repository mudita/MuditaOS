#include "FilesystemEndpoint.hpp"
#include "DesktopMessages.hpp"
#include "ServiceDesktop.hpp"

auto FilesystemEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod())
    {
        case http::Method::post:
            run(context);
            break;
        default:
            break;
    }
}

auto FilesystemEndpoint::run(Context &context) -> sys::ReturnCodes
{
    ServiceDesktop *owner = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
    if (owner) {
        fs::path filePath = context.getBody()[parserFSM::json::fileName].string_value();

        return sys::ReturnCodes::Success;
    }
    return sys::ReturnCodes::Failure;
}