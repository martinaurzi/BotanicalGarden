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
            const float growth_threshold;
            const float death_threshold;
            PlantStatus status;
            const IdealEnvironment ideal_parameters;
            const GrowthFactors temp_factors;
            const GrowthFactors hum_factors;
            const GrowthFactors light_factors;

            // Funzioni membro virtuali pure (= 0) che devono essere accessibili solo alle classi derivate
            virtual void apply_growth(float temp, float hum, float light) = 0;
            virtual void update_health(float temp, float hum, float light) = 0;

            // Funzione statica condivisa tra tutte le istanze
            static float calculate_environment_factor(float current_value, float min_value, float max_value, const GrowthFactors& factors);

            bool is_environment_ideal(float temp, float hum, float light) const;

        public:
            // Costruttore
            Plant(const std::string& n, const IdealEnvironment& ideal, float growth_th, float death_th,
                const GrowthFactors& temp_f, const GrowthFactors& hum_f, const GrowthFactors& light_f);

            // Distruttore
            virtual ~Plant() = default;

            void update_plant_status(float temp, float hum, float light); // dovrei passare quello che mi manda Go

            virtual std::string get_plant_type() const = 0;

            bool is_dead() const;

            GrowthStage get_growth_stage() const;
            std::string get_growth_stage_str() const;
            std::string get_plant_name() const;
            PlantStatus get_plant_status() const;
            float get_growth_threshold() const; // NON USATA
            float get_death_threshold() const; // NON USATA

            virtual std::string printPlant() const;
    };
}

#endif