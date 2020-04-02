#include "InputEvent.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
namespace gui
{

    /*
     *
     */
    class KeyProfile
    {
      public:
        uint32_t keyCode = 0;
        bool cyclic      = false;
        ;
        std::vector<uint32_t> chars;
        std::vector<uint32_t> timeouts;

        KeyProfile();
        virtual ~KeyProfile();

        void addCharacters(const std::string &s);
        void addTimeouts(const std::string &s);
    };

    /*
     *
     */
    class Profile
    {
        std::string name;
        std::map<uint32_t, KeyProfile *> keys = {};

        void addCharacters(KeyProfile *pk, const std::string &s);
        void addTimeouts(KeyProfile *pk, const std::string &s);
        void addKeyProfile(KeyProfile *pk);
        void setName(std::string name)
        {
            this->name = name;
        };
        const KeyProfile *getKeyProfile(uint32_t keyCode);

      public:
        void clear()
        {
            this->keys.clear();
        };
        Profile() = default;
        Profile(const std::string &name);
        Profile(Profile &&p);
        virtual ~Profile();

        std::string getName()
        {
            return name;
        };
        bool load(std::string filename);

        uint32_t get(bsp::KeyCodes code, uint32_t times);
    };

} /* namespace gui */
