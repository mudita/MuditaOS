#pragma once
#include "BtProfile.hpp"

/// generic audio video class
class GAVD : public BtProfile {
    public:
        GAVD();
        virtual ErrorBtProfile init(Stack *stack) override;
};
