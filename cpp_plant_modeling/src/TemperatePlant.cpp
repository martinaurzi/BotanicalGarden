#include <format>
#include <iostream>

#include "TemperatePlant.h"

namespace BotanicalGarden
{
    TemperatePlant::TemperatePlant(const std::string& n, const IdealEnvironment& ideal, const float cold_dorm, const float low_light_t, const float leaf_renewal):
        Plant(n, ideal, 50.0f, 25.0f, {0.6f, 0.0f, 0.4f}, {0.5f, 0.1f, 0.1f},
            {0.5f, 0.3f, 0.2f}), cold_dormancy(cold_dorm), low_light_tolerance(low_light_t), leaf_renewal(leaf_renewal){}

    void TemperatePlant::apply_damage_with_shield(const float base_damage)
    {
        constexpr float damage_scaling{0.2f};
        const float damage_factor = (isDormant && shield_charge > 0.0f) ? (1.0f - shield_charge) : 1.0f;

        if (base_damage <= 0.0f)
            return;

        // Applichiamo il danno
        status.health -= base_damage * damage_factor * damage_scaling;

        if (isDormant)
        {
            // Lo scudo si consuma
            shield_charge = std::max(0.0f, shield_charge - 0.2f);

            std::cout << std::format("Protezione dello scudo dopo il danno: {:.2f}%", shield_charge * 100.0f) << std::endl;
        }
    }

    void TemperatePlant::apply_seasonal_effects(const Season season)
    {
        switch (season)
        {
            case Season::Spring:
                // Il rinnovo delle foglie permette alla pianta di recuperare quasi la totalità della salute
                // Esempio: status.health=80, missing_health=20, se leaf_renewal=0.8 --> status.health = 80 + 16 = 96
                if (status.health < 100.0f)
                {
                    const float missing_health{100.0f - status.health};
                    status.health = std::min(100.0f, status.health + missing_health * leaf_renewal);
                }
                break;

            case Season::Winter:
                // In inverno la pianta entra in riposo vegetativo e perde meno salute
                if (!isDormant)
                {
                    isDormant = true;
                    shield_charge = std::min(0.90f, cold_dormancy + 0.05f);
                }
                break;

            default:
                break;
        }
    }

    void TemperatePlant::update_health(const float temp, const float hum, const float light, const Season season)
    {
        constexpr float damage_intensity{1.5f};

        if (season != Season::Winter)
            isDormant = false;

        apply_seasonal_effects(season);

        // Condizioni ideali: la pianta recupera salute
        if (is_environment_ideal(temp, hum, light))
        {
            status.health = std::min(100.0f, status.health + 5.0f);
            std::cout << "CONDIZIONI IDEALI" << std::endl;
            return;
        }

        // Notte
        if (constexpr float night_temp_threshold{15.0f}; light <= ideal_parameters.light.min && temp <= night_temp_threshold)
        {
            // La pianta entra in riposo vegetativo, perde meno salute fino a quando lo "scudo" non si consuma
            if (!isDormant && cold_dormancy > 0.0f)
            {
                isDormant = true;
                shield_charge = cold_dormancy;

                std::cout << std::format("La pianta {} e' entrata in riposo vegetativo (resistenza ai danni: {:.2f}%)", name, shield_charge * 100.0f) << std::endl;
            }
        }

        if (temp > ideal_parameters.temperature.max)
        {
            // Temperatura elevata: danno grave
            const float base_damage_temp_max = (temp - ideal_parameters.temperature.max) * damage_intensity;

            std::cout << "Danno temp > max" << std::endl;
            apply_damage_with_shield(base_damage_temp_max);
        }
        else if (temp < ideal_parameters.temperature.min)
        {
            const float base_damage_temp_min = (1.0f - cold_dormancy) * (ideal_parameters.temperature.min - temp);

            std::cout << "Danno temp < min" << std::endl;
            apply_damage_with_shield(base_damage_temp_min);
        }

        if (hum > ideal_parameters.humidity.max)
        {
            const float base_damage_hum_max = (hum - ideal_parameters.humidity.max) * damage_intensity;

            std::cout << "Danno hum > max" << std::endl;
            apply_damage_with_shield(base_damage_hum_max);
        }
        else if (hum < ideal_parameters.humidity.min)
        {
            // Umidità bassa: terreno troppo secco
            const float base_damage_hum_min = (ideal_parameters.humidity.min - hum);

            std::cout << "Danno hum < min" << std::endl;
            apply_damage_with_shield(base_damage_hum_min);
        }

        if (light > ideal_parameters.light.max)
        {
            // Luce elevata:
            const float base_damage_light_max = (light - ideal_parameters.light.max) * damage_intensity;

            std::cout << "Danno light > max" << std::endl;
            apply_damage_with_shield(base_damage_light_max);
        }
        else if (light < ideal_parameters.light.min)
        {
            // Luce bassa
            const float base_damage_light_min = (ideal_parameters.light.min - light) * (1.0f - low_light_tolerance);

             std::cout << "Danno light < min" << std::endl;
            apply_damage_with_shield(base_damage_light_min);
        }

        if (isDormant && shield_charge <= 0.0f)
        {
            // Lo scudo è stato esaurito
            isDormant = false;
            std::cout << std::format("La pianta {} si e' risvegliata dopo il riposo vegetativo (scudo: {:.2f}%)", name, shield_charge * 100.0f) << std::endl;
        }

        // Controllo per evitare che la saluta scenda sotto lo 0
        status.health = std::max(0.0f, status.health);
    }

    void TemperatePlant::apply_growth(const float temp, const float hum, const float light)
    {
        if (isDormant)
        {
            std::cout << std::format("La pianta {} e' in riposo vegetativo: crescita temporaneamente bloccata.", name) << std::endl;
            return;
        }

        const float temp_factor = calculate_environment_factor(
            temp,
            ideal_parameters.temperature.min,
            ideal_parameters.temperature.max,
            temp_factors
            );

        const float hum_factor = calculate_environment_factor(
            hum,
            ideal_parameters.humidity.min,
            ideal_parameters.humidity.max,
            hum_factors
            );

        const float light_factor = calculate_environment_factor(
            light,
            ideal_parameters.light.min,
            ideal_parameters.light.max,
            light_factors
            );

        const float sum_factors = temp_factor + hum_factor + light_factor;

        status.growth = std::min(10.0f, status.growth + sum_factors);
    }

    std::string TemperatePlant::get_plant_type() const
    {
        return "Temperate";
    }

    std::string TemperatePlant::printPlant() const
    {
        return  std::format("Type: {}\n", get_plant_type()) +
                Plant::printPlant() +
                std::format("Cold Dormancy: {:.2f}\n"
                            "Low light tolerance: {:.2f}\n"
                            "Leaf renewal: {:.2f}\n",
                            cold_dormancy,
                            low_light_tolerance,
                            leaf_renewal
                );
    }
}