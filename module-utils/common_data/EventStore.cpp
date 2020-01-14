#include "EventStore.hpp"

namespace Store
{
    // if it grows bigger than these few variables - consider moving it to ram with i.e.
    // delayed construction singletone
    Battery battery;

    const Battery &Battery::get()
    {
        return battery;
    }

    Battery &Battery::modify()
    {
        return battery;
    }

}; // namespace Store
