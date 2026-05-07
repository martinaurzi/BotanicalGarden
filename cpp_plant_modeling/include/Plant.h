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
            const float growth_threshold;
            const float death_threshold;
            PlantStatus status;
            float growth_rate;

        public:
            // Costruttore
            Plant(const std::string& n, const IdealEnvironment& ideal, float growth_th, float death_th,
                float growth_r = 1.0f);

            // Distruttore
            virtual ~Plant() = default;

            void update_plant_status(float temp, float hum, float light); // dovrei passare quello che mi manda Go

            // Funzione per ottenere lo stage attuale della pianta: Germoglio, Piantina, Adulta
            GrowthStage get_growth_stage() const;

            std::string get_growth_stage_str() const;

            std::string getPlantName() const;

            PlantStatus getPlantStatus() const;

            virtual std::string toString() const;

        protected:
            // Funzioni membro virtuali pure (= 0) che devono essere accessibili solo alle classi derivate
            virtual void apply_growth(float temp, float hum, float light) = 0;
            virtual void update_health(float temp, float hum, float light) = 0;
            virtual float get_temp_factor(float temp) = 0;
            virtual float get_hum_factor(float hum) = 0;
            virtual float get_light_factor(float light) = 0;
    };
}

#endif //BOTANICALGARDEN_PLANT_H