#include <Bluetooth/profiles/GAP.hpp>

class GAPi :public GAP {
    public:
        GAPi();
        virtual ErrorBtProfile init(Stack *stack) override;
        virtual ErrorBtProfile scan() override;
};

GAPi::GAPi()
{
}

BtProfile::ErrorBtProfile GAPi::init(Stack *stack)
{
    return BtProfile::ErrorNotImplemented;
}

BtProfile::ErrorBtProfile GAPi::scan()
{
    return BtProfile::ErrorNotImplemented;
}

GAP *GAP::create()
{
    return new GAPi();
}
