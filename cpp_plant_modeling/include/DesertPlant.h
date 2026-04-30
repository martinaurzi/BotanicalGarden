#ifndef BOTANICALGARDEN_DESERTPLANT_H
#define BOTANICALGARDEN_DESERTPLANT_H

#include "Plant.h"

namespace BotanicalGarden
{
    class DesertPlant final : public Plant
    {
        private:
            float water_reserve;

        public:
            DesertPlant(const std::string& n, const IdealEnvironment& ideal, float water_r = 0.0f);

            /* override utilizzato per marcare esplicitamente che una funzione membro in una
             * classe derivata è destinata a sovrascrivere una funzione virtual della classe base.
             * In questo modo, se dimentico di mettere virtual ottengo un warning
            */
            void update_growth(float t, float h, float l) override;

            std::string toString() const override;
    };
}

#endif //BOTANICALGARDEN_DESERTPLANT_H