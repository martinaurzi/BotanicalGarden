#ifndef BOTANICALGARDEN_TROPICALPLANT_H
#define BOTANICALGARDEN_TROPICALPLANT_H

#include <vector>
#include <memory>
#include <limits>

#include "Plant.h"
#include "GardenManager.h"

namespace BotanicalGarden
{
    class TropicalPlant final : public Plant
    {
        float direct_light_tolerance;
        float thermal_shock_vulnerability;
        float prev_temperature{std::numeric_limits<float>::infinity()};
        int ideal_env_streak{0};
        static constexpr float thermal_shock_threshold = 10.0f;
        static constexpr float max_direct_light_tolerance = 1.0f;

        public:
            TropicalPlant(const std::string& n, const IdealEnvironment& ideal, float direct_light_t = 0.6f, float thermal_shock_v = 0.3f);

            void apply_growth(float temp, float hum, float light) override;

            void update_health(float temp, float hum, float light, Season season) override;

            std::string get_plant_type() const override;

            std::string printPlant() const override;

        private:
            // inline
            void set_direct_light_tolerance(const float new_value) {direct_light_tolerance = new_value;}
            void set_thermal_shock_vulnerability(const float new_value) {thermal_shock_vulnerability = new_value;}

            friend void GardenManager::load_garden_from_file(const std::string& filename);
    };
}
#endif //BOTANICALGARDEN_TROPICALPLANT_H