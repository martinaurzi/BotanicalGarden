#include "DesertPlant.h"

#include <cmath>
#include <format>
#include <iostream>

namespace BotanicalGarden
{
    DesertPlant::DesertPlant(const std::string& n, const IdealEnvironment& ideal):
        Plant(n, ideal, 60.0f, 20.0f, {0.4f, 0.05f, 0.1f},
            {0.3f, 0.0f, 0.2f}, {0.5f, 0.5f, 0.0f}),
            water_reserve{0.0f}
    {}

    void DesertPlant::use_water_reserve(float& water_reserve_protection)
    {
        constexpr float needed_water{1.0f};
        const float water_ratio = water_reserve / needed_water;

        /* Se water_reserve > needed_water, ad esempio, water_reserve=2.0f, 1.0f - (water_reserve / needed_water) risulta negativo
         * quindi verrà preso il massimo tra i due valori, cioè 0.0f. In questo caso la pianta non subisce danni perchè utilizza la riserva d'acqua.
         * Se invece water_reserve=0.2f, 1.0f - water_ratio = 0.8f e la pianta subisce un danno proporzionale a questo valore
         */
        water_reserve_protection = std::max(0.0f, 1.0f - water_ratio);

        /* Se water_reserve > needed_water allora decremento la water_reserve di una quantità pari a needed_water.
         * Se water_reserve < needed_water allora pongo water_reserve=0.0 per non aver un valore negativo */
        water_reserve = std::max(0.0f, water_reserve - needed_water);
    }

    // Le piante desertiche sono molto resistenti al caldo e alla siccità dell'aria (curve logaritmiche), ma sono estremamente vulnerabili al
    // freddo (curva con radice quadrata) e all'umidità elevata (logaritmo con argomento al cubo).
    void DesertPlant::update_health(float temp, float hum, float light)
    {
        constexpr float damage_scaling{0.4f};
        float water_reserve_protection{1.0f};

        // Condizioni ideali: la pianta recupera salute
        if (temp >= ideal_parameters.temperature.min && temp <= ideal_parameters.temperature.max &&
            hum >= ideal_parameters.humidity.min && hum <= ideal_parameters.humidity.max &&
            light >= ideal_parameters.light.min && light <= ideal_parameters.light.max)
        {
            status.health = std::min(100.0f, status.health + 3.0f);
            water_reserve = std::min(max_water_reserve, water_reserve + 0.2f);
            std::cout << "CONDIZIONI IDEALI" << std::endl;

            return;
        }

        // Temperatura elevata: la pianta soffre, ma non perde salute se ha riserva d'acqua
        if (temp > ideal_parameters.temperature.max)
        {
            const float temp_delta = temp - ideal_parameters.temperature.max;

            use_water_reserve(water_reserve_protection);

            status.health -= std::log1p(temp_delta) * water_reserve_protection;
        }
        else if (temp < ideal_parameters.temperature.min)
        {
            const float temp_delta = ideal_parameters.temperature.min - temp;

            // Il freddo è letale per una pianta desertica
            status.health -= temp_delta * std::sqrt(temp_delta) * damage_scaling;
        }

        // Umidità elevata: danno considerevole per la pianta, ma la riserva d'acqua aumenta
        if (hum > ideal_parameters.humidity.max)
        {
            const float hum_delta_max = hum - ideal_parameters.humidity.max;

            status.health -= std::log1p(hum_delta_max * hum_delta_max * hum_delta_max);

            water_reserve = std::min(max_water_reserve, water_reserve + 1.0f);
        }
        else if (hum < ideal_parameters.humidity.min)
        {
            const float hum_delta_min = ideal_parameters.humidity.min - hum;

            // Umidità bassa: se la pianta possiede abbastanza riserva d'acqua, il danno viene azzerato, sennò il danno viene ridotto in modo
            // proporzionale alla riserva d'acqua disponibile.
            use_water_reserve(water_reserve_protection);

            status.health -= std::log1p(hum_delta_min) * water_reserve_protection * damage_scaling;
        }

        // Luce bassa: danno leggero
        if (light < ideal_parameters.light.min)
        {
            const float light_delta = ideal_parameters.light.min - light;

            status.health -= std::log1p(light_delta) * damage_scaling;
        }

        // Controllo per evitare che la saluta scenda sotto lo 0
        status.health = std::max(0.0f, status.health);
    }

    void DesertPlant::apply_growth(const float temp, const float hum, const float light)
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

        // Simuliamo una crescita lenta utilizzando la radice quadrata. Controllo se l'argomento della radice quadrata è maggiore di 1
        if (const float sum_factors = temp_factor + hum_factor + light_factor; sum_factors > 1.0f)
            status.growth = std::min(10.0f, status.growth + std::sqrt(sum_factors));
        else
            status.growth = std::min(10.0f, status.growth + sum_factors);
    }

    std::string DesertPlant::printPlant() const
    {
        return Plant::printPlant() + std::format("Water reserve: {:.2f}\n", water_reserve);
    }
};