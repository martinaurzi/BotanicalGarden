#include "DesertPlant.h"

namespace BotanicalGarden
{
    DesertPlant::DesertPlant(const std::string& n, const IdealEnvironment& ideal, const float water_r):
        Plant(n, ideal, 0.5f), water_reserve(water_r){}

    void DesertPlant::update_growth(float t, float h, float l)
    {
        // Da implementare
    }

    std::string DesertPlant::toString() const
    {
        return Plant::toString() + "Water reserve: " + std::to_string(water_reserve);
    }
} // BotanicalGarden