#include <string>
#include <cctype>

namespace BotanicalGarden
{
    bool contains_letters(const std::string &s)
    {
        for (const char c : s)
        {
            if (isalpha(c))
                return true;
        }

        return false;
    }
}
