#include <cmath>

#include "TropicalPlant.h"

#include <format>
#include <iostream>

namespace BotanicalGarden
{
    TropicalPlant::TropicalPlant(const std::string& n, const IdealEnvironment& ideal, const float direct_light_t, const float thermal_shock_v):
        Plant(n, ideal, 75.0f, 45.0f, {0.5f, 0.2f, 0.0f},
        {0.6f, 0.4f, 0.05f}, {0.4f, 0.0f, 0.1f}),
                direct_light_tolerance(direct_light_t), thermal_shock_vulnerability(thermal_shock_v){}

    void TropicalPlant::update_health(const float temp, const float hum, const float light)
    {
        constexpr float damage_scaling{0.1f};

        // Calcolo dello shock termico rispetto al ciclo precedente
        if (std::isfinite(prev_temperature))
        {
            const float delta_temp_prev = std::abs(temp - prev_temperature);

            if (delta_temp_prev >= thermal_shock_threshold)
            {
                const float delta_shock = delta_temp_prev - thermal_shock_threshold;
                std::cout << "Salute prima shock termico: " << status.health << std::endl;
                status.health -= std::pow(delta_shock, 2.0f) * thermal_shock_vulnerability * damage_scaling;
                std::cout << "Salute dopo shock termico: " << status.health << std::endl;
            }
        }

        // Controlli per aggiornare la tolleranza alla luce diretta
        if (light > ideal_parameters.light.max)
        {
            // Luce troppo alta: la tolleranza alla luce diretta diminuisce
            direct_light_tolerance = std::max(0.0f, direct_light_tolerance - (light - ideal_parameters.light.max) * 0.1f);
        }
        else
        {
            // La tolleranza si rigenera in tutti gli altri casi tranne quando l'aria è secca e la temperatura è elevata
            if (hum < ideal_parameters.humidity.min && temp > ideal_parameters.temperature.max)
            {
                direct_light_tolerance = std::max(0.0f, direct_light_tolerance - 0.1f);
            }
            else
            {
                direct_light_tolerance = std::min(1.0f, direct_light_tolerance + 0.05f);
            }
        }

        /* Condizioni ideali: la salute della pianta aumenta
         * - Se le condizioni sono ideale per più turni consecutivi, la salute della pianta cresce ulteriormente
         */
        if (temp >= ideal_parameters.temperature.min && temp <= ideal_parameters.temperature.max &&
            hum >= ideal_parameters.humidity.min && hum <= ideal_parameters.humidity.max &&
            light >= ideal_parameters.light.min && light <= ideal_parameters.light.max)
        {
            std::cout << "CONDIZIONI IDEALI" << std::endl;

            ideal_env_streak++;

            const float health_boost = std::min(10.0f, std::pow(2.0f, static_cast<float>(ideal_env_streak)));

            status.health = std::min(100.0f, status.health + health_boost);

            prev_temperature = temp;

            return;
        }

        ideal_env_streak = 0;

        // Temperatura elevata e umidità elevata: la salute della pianta aumenta leggermente
        if (temp > ideal_parameters.temperature.max && hum > ideal_parameters.humidity.max)
        {
            const float extra_temp = temp - ideal_parameters.temperature.max;
            const float extra_hum = hum - ideal_parameters.humidity.max;

            // La tanh, per valori positivi, restituisce valori compresi tra 0 e 1
            status.health = std::min(100.0f, status.health + std::tanh(extra_temp) + std::tanh(extra_hum));
        }

        // Temperatura molto bassa: la pianta soffre il freddo
        if (temp < ideal_parameters.temperature.min)
        {
            const float delta_temp = ideal_parameters.temperature.min - temp;

            status.health -= std::log(1.0f + delta_temp) * damage_scaling;
        }

        if (light > ideal_parameters.light.max)
        {
            if (direct_light_tolerance > 0.0f)
            {
                // Il danno alla salute viene mitigato da direct_light_tolerance
                status.health -= (light - ideal_parameters.light.max) * (1.0f - direct_light_tolerance) * damage_scaling;
            }
            else
            {
                status.health -= (light - ideal_parameters.light.max) * damage_scaling;
            }
        }

        // Umidità bassa: danno alla salute cresce velocemente
        if (hum < ideal_parameters.humidity.min)
        {
            const float hum_delta = ideal_parameters.humidity.min - hum;

            status.health -= hum_delta * std::sqrt(hum_delta) * damage_scaling;
        }

        prev_temperature = temp;
        status.health = std::max(0.0f, status.health);
    }

    void TropicalPlant::apply_growth(const float temp, const float hum, const float light)
    {
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

        // Crescita esponenziale
        status.growth = std::min(10.0f, status.growth + std::exp(temp_factor + hum_factor + light_factor));
    }

    std::string TropicalPlant::printPlant() const
    {
        return Plant::printPlant() + std::format(
         "Direct light Tolerance: {:.2f}\n"
                "Thermal shock vulnerability: {:.2f}\n"
                "Ideal environment streak: {:d}\n",
                direct_light_tolerance,
                thermal_shock_vulnerability,
                ideal_env_streak
         );
    }
}
