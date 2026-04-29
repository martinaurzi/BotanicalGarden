#include "DesertPlant.h"

namespace BotanicalGarden
{
    DesertPlant::DesertPlant(const std::string& n, const IdealEnvironment& ideal, float water_r):
        Plant(n, ideal, 0.5f), water_reserve(water_r){}

    void DesertPlant::update_growth(float t, float h, float l)
    {
        // Da implementare
    }
} // BotanicalGarden