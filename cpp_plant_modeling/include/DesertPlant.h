#ifndef BOTANICALGARDEN_DESERTPLANT_H
#define BOTANICALGARDEN_DESERTPLANT_H

#include "Plant.h"

namespace BotanicalGarden
{
    class DesertPlant final : public Plant
    {
        float water_reserve;
        static constexpr float max_water_reserve = 10.0f; // static: condiviso tra tutte le DesertPlant, constexpr: valutato a compile-time

        public:
            DesertPlant(const std::string& n, const IdealEnvironment& ideal);

            /* override utilizzato per marcare esplicitamente che una funzione membro in una
             * classe derivata è destinata a sovrascrivere una funzione virtual della classe base.
             * In questo modo, se dimentico di mettere virtual ottengo un warning
            */
            void apply_growth(float temp, float hum, float light) override;

            void update_health(float temp, float hum, float light) override;

            std::string toString() const override;

        private:
            float get_temp_factor(float temp) override;

            float get_hum_factor(float hum) override;

            float get_light_factor(float light) override;

            void use_water_reserve(float&);
    };
}

#endif //BOTANICALGARDEN_DESERTPLANT_H