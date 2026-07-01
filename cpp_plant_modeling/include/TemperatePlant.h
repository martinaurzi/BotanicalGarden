#ifndef BOTANICALGARDEN_TEMPERATEPLANT_H
#define BOTANICALGARDEN_TEMPERATEPLANT_H

#include <memory>
#include <vector>

#include "Plant.h"
#include "GardenManager.h"

namespace BotanicalGarden
{
    class TemperatePlant final : public Plant
    {
        float cold_dormancy;
        float low_light_tolerance;
        float leaf_renewal; // PER ORA NON UTILIZZATO PERCHE SERVE LA PRIMAVERA
        //float season_sensibility; // stagioni
        float shield_charge{0.0f};
        bool isDormant{false};

        public:
            TemperatePlant(const std::string& n, const IdealEnvironment& ideal, float cold_dorm, float low_light_t, float leaf_renewal = 0.0f);

            void update_health(float temp, float hum, float light) override;

            void apply_growth(float temp, float hum, float light) override;

            std::string get_plant_type() const override;

            std::string printPlant() const override;

        private:
            void apply_damage_with_shield(float base_damage);

            void set_leaf_renewal(const float leaf_r) {leaf_renewal = leaf_r;}; // inline

            // Funzione friend che può accedere ai membri private della classe
            friend void GardenManager::load_garden_from_file(const std::string& filename);
    };
}

#endif //BOTANICALGARDEN_TEMPERATEPLANT_H