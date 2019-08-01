#pragma once
#include "BtProfile.hpp"

/// generic audio video class
class GAVD : public BtProfile {
    public:
        ErrorBtProfile init(Stack *stack) =0;
        static GAVD *create();
};
