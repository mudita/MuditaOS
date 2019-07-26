#pragma once
#include "BtProfile.hpp"
#include "GAVD.hpp"

class A2DP : public BtProfile {
    public:
        A2DP();
        virtual ErrorBtProfile init(Stack *stack) override;
};
