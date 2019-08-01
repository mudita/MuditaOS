#pragma once
#include <vector>
#include "../Stack.hpp"

class BtProfile {
    public:
        enum State {
            ProfileInitialized,
            ProfileActive,
            ProfileInactive
        };
        enum Profiles {
            GAVD,
            A2DP,
        };
        enum ErrorBtProfile {
            SuccessBtProfile,
            ErrorGenericBtProfile,
        };
        BtProfile() {};
        ~BtProfile() {};
        // should take stack as param and initialize all other needed profiles
        virtual ErrorBtProfile init(Stack *stack) = 0;
        State st;
        std::vector<Profiles> depends;
};
