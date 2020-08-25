#include "Endpoint.hpp"
#include "ParserUtils.hpp"
#include "ServiceDesktop.hpp"

using namespace ParserStateMachine;

class RestoreEndpoint : public Endpoint
{
  public:
    RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "RestoreEndpoint";
    }
    auto handle(Context &context) -> std::string override;
};
