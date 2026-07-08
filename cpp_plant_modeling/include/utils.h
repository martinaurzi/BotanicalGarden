#ifndef BOTANICALGARDEN_UTILS_H
#define BOTANICALGARDEN_UTILS_H

#include <string>
#include <unordered_map>

#include "Types.h"

namespace  BotanicalGarden
{
    bool contains_letters(const std::string &s);

    template <typename T>
    std::string enum_to_string(T key, const std::unordered_map<T, std::string>& map)
    {
        // Cerchiamo la chiave dentro la mappa. .find() torna un iteratore
        auto it = map.find(key);

        // Verifichiamo se la chiave è stata trovata e in tal caso restituiamo la stringa corrispondente
        if (it != map.end())
        {
            return it->second; // second = valore
        }

        return "Unknown";
    }

    std::string growth_stage_to_string(GrowthStage stage);

    std::string season_to_string(Season season);
}
#endif //BOTANICALGARDEN_UTILS_H