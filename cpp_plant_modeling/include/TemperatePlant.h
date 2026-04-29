#ifndef BOTANICALGARDEN_TEMPERATEPLANT_H
#define BOTANICALGARDEN_TEMPERATEPLANT_H

#include "Plant.h"

namespace BotanicalGarden
{
    class TemperatePlant final : public Plant
    {
        private:
            float healing_rate;

        public:
            TemperatePlant(const std::string& n, const IdealEnvironment& ideal, float healing_r = 1.0f);

            void update_growth(float t, float h, float l) override;
    };
}

#endif //BOTANICALGARDEN_TEMPERATEPLANT_H