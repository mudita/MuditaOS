#include "Bluetooth/profiles/GAVD.hpp"

class GAVDi : public GAVD {
    public:
    GAVDi();
    ErrorBtProfile init(Stack *stack);
};

GAVDi::GAVDi()
{
}

BtProfile::ErrorBtProfile GAVDi::init(Stack *stack)
{
    return ErrorBtProfile::ErrorGenericBtProfile;
}

GAVD* GAVD::create()
{
    return new GAVDi();
}
