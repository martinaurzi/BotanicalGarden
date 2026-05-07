#include "DesertPlant.h"

namespace BotanicalGarden
{
    DesertPlant::DesertPlant(const std::string& n, const IdealEnvironment& ideal):
        Plant(n, ideal, 60, 20, 0.3f), water_reserve{0.0f}{}

    void DesertPlant::use_water_reserve(float& water_reserve_protection)
    {
        constexpr float needed_water{1.0f};

        /* Se water_reserve > needed_water, ad esempio, water_reserve=2.0f, 1.0f - (water_reserve / needed_water risulta negativo
         * quindi verrà preso il massimo tra i due valori, cioè 0.0f. In questo caso la pianta non subisce danni perchè utilizza
         * la riserva d'acqua.
         *
         * Se invece water_reserve=0.2f, 1.0f - (water_reserve / needed_water) = 0.8f e la pianta subisce un danno proporzionale
         * a questo valore */
        water_reserve_protection = std::max(0.0f, 1.0f - (water_reserve / needed_water));

        /* Se water_reserve > needed_water allora decremento la water_reserve di una quantità pari a needed_water.
         * Se water_reserve < needed_water allora pongo water_reserve=0.0 per non aver un valore negativo */
        water_reserve = std::max(0.0f, water_reserve - needed_water);
    }

    void DesertPlant::update_health(float temp, float hum, float light)
    {
        constexpr float damage_intensity{2.0f}; // fattore per far diminuire la salute più velocemente in casi estremi
        constexpr float damage_scaling{0.1f}; // usato per calcolare health per evitare danni troppo considerevoli
        float water_reserve_protection{1.0f};
        //float remaining_water{water_reserve};

        // Condizioni ideali: la pianta recupera salute
        if (temp > ideal_parameters.temperature.min && temp < ideal_parameters.temperature.max &&
            hum > ideal_parameters.humidity.min && hum < ideal_parameters.humidity.max &&
            light > ideal_parameters.light.min && light < ideal_parameters.light.max)
        {
            status.health = std::min(100.0f, status.health + 5.0f);
            water_reserve = std::min(max_water_reserve, water_reserve + 0.2f);
            return;
        }

        // Temperatura elevata: la pianta soffre, ma ma non perde salute se ha riserva d'acqua
        if (temp >= ideal_parameters.temperature.max)
        {
            const float temp_delta = temp - ideal_parameters.temperature.max;

            use_water_reserve(water_reserve_protection);

            status.health -= damage_intensity * temp_delta * water_reserve_protection * damage_scaling;

            /* Alternativa più semplice:
            if (water_reserve >= needed_water)
            {
                water_reserve--;
            }
            else
            {
                remaining_water = 1.0f - (water_reserve / needed_water);
                water_reserve = 0.0f;
                status.health -= damage_intensity * temp_delta * remaining_water * damage_scaling;
            }*/
        }
        else if (temp <= ideal_parameters.temperature.min)
        {
            status.health -= 1.0f;
        }

        // Umidità elevata: la pianta soffre, ma la riserva d'acqua aumenta
        if (hum >= ideal_parameters.humidity.max)
        {
            status.health -= damage_intensity * (hum - ideal_parameters.humidity.max) * damage_scaling;
            water_reserve = std::min(max_water_reserve, water_reserve + 2.0f);
        }
        else if (hum <= ideal_parameters.humidity.min)
        {
            // Umidità bassa: se la pianta possiede abbastanza riserva d'acqua, il danno viene azzerato, sennò il danno viene ridotto
            // in modo proporzionale alla riserva d'acqua disponibile.
            use_water_reserve(water_reserve_protection);

            status.health -= 0.5f * water_reserve_protection;
        }

        // Luce bassa: danno leggero
        if (light < ideal_parameters.light.min)
        {
            status.health -= 0.2f;
        }
    }

    float DesertPlant::get_temp_factor(float temp){

        return (temp > ideal_parameters.temperature.min && temp < ideal_parameters.temperature.max) ? 0.4f :
               (temp >= ideal_parameters.temperature.max) ? 0.05f : 0.1f;
    }

    float DesertPlant::get_hum_factor(float hum)
    {
        return (hum > ideal_parameters.humidity.min && hum < ideal_parameters.humidity.max) ? 0.3f :
               (hum >= ideal_parameters.humidity.max) ? 0.0f : 0.2f;
    }

    float DesertPlant::get_light_factor(float light)
    {
        return (light > ideal_parameters.light.min && light < ideal_parameters.light.max) ? 0.5f :
               (light >= ideal_parameters.light.max) ? 0.5f : 0.0f;
    }

    void DesertPlant::apply_growth(float temp, float hum, float light)
    {
        DesertPlant::update_health(temp, hum, light);

        const float temp_factor = DesertPlant::get_temp_factor(temp);
        const float hum_factor = DesertPlant::get_hum_factor(hum);
        const float light_factor = DesertPlant::get_light_factor(light);

        status.growth += (growth_rate) * (temp_factor + hum_factor + light_factor);
    }

    std::string DesertPlant::toString() const
    {
        return Plant::toString() + "Water reserve: " + std::to_string(water_reserve);
    }
} // BotanicalGarden