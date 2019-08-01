#pragma once

#include "BtProfile.hpp"

class GAP : public BtProfile {
    public:
        virtual ErrorBtProfile init(Stack *stack) =0;
        virtual ErrorBtProfile scan() =0;
        static GAP *create();
};

