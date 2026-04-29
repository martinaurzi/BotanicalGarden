#include "TropicalPlant.h"

namespace BotanicalGarden
{
    TropicalPlant::TropicalPlant(const std::string& n, const IdealEnvironment& ideal, float direct_light_t):
        Plant(n, ideal, 1.5f), direct_light_tolerance(direct_light_t){}

    void TropicalPlant::update_growth(float t, float h, float l)
    {
        // Da implementare
    }
}