#include "TemperatePlant.h"

namespace BotanicalGarden
{
    TemperatePlant::TemperatePlant(const std::string& n, const IdealEnvironment& ideal, const float healing_r):
        Plant(n, ideal, 70, 40), healing_rate(healing_r){}

    void TemperatePlant::apply_growth(float temp, float hum, float light)
    {
        // Da implementare
    }
}