#ifndef BOTANICALGARDEN_TROPICALPLANT_H
#define BOTANICALGARDEN_TROPICALPLANT_H

#include "Plant.h"

namespace BotanicalGarden
{
    class TropicalPlant final : public Plant
    {
        float direct_light_tolerance;

        public:
            TropicalPlant(const std::string& n, const IdealEnvironment& ideal, float direct_light_t = 0.8f);

            void apply_growth(float temp, float hum, float light) override;
    };
}

#endif //BOTANICALGARDEN_TROPICALPLANT_H