#include "TropicalPlant.h"

namespace BotanicalGarden
{
    TropicalPlant::TropicalPlant(const std::string& n, const IdealEnvironment& ideal, const float direct_light_t):
        Plant(n, ideal, 85, 60, 1.5f), direct_light_tolerance(direct_light_t){}

    void TropicalPlant::apply_growth(float temp, float hum, float light)
    {
        // Da implementare
    }
}