#ifndef BOTANICALGARDEN_TROPICALPLANT_H
#define BOTANICALGARDEN_TROPICALPLANT_H

#include "Plant.h"

namespace BotanicalGarden
{
    class TropicalPlant final : public Plant
    {
        private:
            float direct_light_tolerance;

        public:
            TropicalPlant(const std::string& n, const IdealEnvironment& ideal, float direct_light_t = 0.8f);

            void update_growth(float t, float h, float l) override;
    };
}

#endif //BOTANICALGARDEN_TROPICALPLANT_H