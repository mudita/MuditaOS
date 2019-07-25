/*
 *  @file State.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_STATE_HPP
#define PUREPHONE_STATE_HPP

#include <memory>
#include <optional>
#include "../Operation/Operation.hpp"

class Profile;

class State {
public:
    enum class Type {
        Idle,
        Playback,
        Recording,
        Routing,
    };


    static std::optional<std::unique_ptr<State>> Create(Type t, const char *fileName, const Profile *prof);

    State(Type t, Operation::Type ot, const char *fileName, const Profile *prof);

    virtual int32_t SwitchProfile(const Profile *prof) = 0;

    virtual bool IsSwitchPossible(const Type t) = 0;

    Type GetType() const {return type;}
    const Profile* GetProfile() const {return profile;}


protected:
    std::unique_ptr<Operation> operation;
    const Profile *profile;
    const Type type;

};


#endif //PUREPHONE_STATE_HPP
