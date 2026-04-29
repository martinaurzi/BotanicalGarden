#ifndef BOTANICALGARDEN_PLANT_H
#define BOTANICALGARDEN_PLANT_H

#include <string>
#include "Types.h"

namespace BotanicalGarden
{
    class Plant
    {
        protected:
            const std::string name;
            const IdealEnvironment ideal_parameters;
            PlantStatus status;
            float growth_rate;

        public:
            // Costruttore
            Plant(const std::string& n, const IdealEnvironment& ideal, float growth_r = 1.0f);

            // Distruttore
            virtual ~Plant() = default;

            // Funzione membro virtuale pura (= 0)
            virtual void update_growth(float t, float h, float l) = 0; // dovrei passare quello che mi manda Go

            // Funzione per ottenere lo stage attuale della pianta: Germoglio, Piantina, Adulta
            GrowthStage get_growth_stage() const;
    };
}

#endif //BOTANICALGARDEN_PLANT_H