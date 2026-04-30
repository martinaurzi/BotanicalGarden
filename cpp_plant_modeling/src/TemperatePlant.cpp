#include "TemperatePlant.h"

namespace BotanicalGarden
{
    TemperatePlant::TemperatePlant(const std::string& n, const IdealEnvironment& ideal, const float healing_r):
        Plant(n, ideal), healing_rate(healing_r){}

    void TemperatePlant::update_growth(float t, float h, float l)
    {
        // Da implementare
    }
}