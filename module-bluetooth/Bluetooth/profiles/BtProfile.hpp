#pragma once
#include <vector>
#include <string>
#include "../Stack.hpp"

class BtProfile {
    public:
        enum State {
            ProfileInitialized,
            ProfileActive,
            ProfileInactive
        };
        enum Profiles {
            GAP,
            GAVD,
            A2DP,
        };
        enum ErrorBtProfile {
            SuccessBtProfile,
            ErrorGenericBtProfile,
            ErrorNotImplemented,
        };

        BtProfile() {};
        virtual ~BtProfile(){};
        virtual ErrorBtProfile init(Stack *stack) = 0; // should take stack as param and initialize all other needed profiles
        State st;
        std::vector<Profiles> depends;
};
