#include "Endpoint.hpp"
#include "ParserUtils.hpp"
#include "ServiceDesktop.hpp"

using namespace parserFSM;

class RestoreEndpoint : public Endpoint
{
  public:
    RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "RestoreEndpoint";
    }
    auto handle(Context &context) -> void override;
};
