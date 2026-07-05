#include "Plant.h"
#include "utils.h"

#include <iostream>
#include <format>
#include <stdexcept>

namespace BotanicalGarden
{
    // Implementazione del costruttore usando MIL. const&: passo per riferimento per evitare la copia
    Plant::Plant(const std::string& n, const IdealEnvironment& ideal, const float growth_th, const float death_th,
        const GrowthFactors& temp_f, const GrowthFactors& hum_f, const GrowthFactors& light_f) :
        name(n), growth_threshold(growth_th), death_threshold(death_th), status{0.0f, 80.0f},
        ideal_parameters(ideal), temp_factors(temp_f), hum_factors(hum_f), light_factors(light_f)
    {
        if (!contains_letters(n))
            throw std::invalid_argument("Il nome della pianta deve contere lettere");

        if (ideal.temperature.max < ideal.temperature.min)
            throw std::invalid_argument("Temperatura massima < Temperatura minima");

        if (ideal.humidity.max < ideal.humidity.min)
            throw std::invalid_argument("Umidita' massima < Umidita' minima");

        if (ideal.light.max < ideal.light.min)
            throw std::invalid_argument("Luce massima < Luce minima");

        if (death_th > growth_th)
            throw std::invalid_argument("La soglia di morte deve essere minore della soglia di crescita");
    }

    bool Plant::is_dead() const noexcept
    {
        return status.health <= death_threshold;
    }

    /* Nella funzione update_growth viene implementata la logica di crescita comune a tutte le piante. In particolare, prima di aggiornare la crescita si verifica la salute della pianta.
     * Se la salute della pianta si mantiene sopra una determinata soglia, la pianta cresce. Quando la salute si trova in uno stato intermedio, la pianta interrompe la crescita.
     * Se lo stato di salute si trova sotto la soglia critica, la pianta muore. */
    void Plant::update_plant_status(const float temp, const float hum, const float light, const Season season)
    {
        // Pianta morta
        if (is_dead())
        {
            std::cout << std::format("La pianta {} e' morta (salute: {:.2f}% <= {:.2f}%)", name, status.health, death_threshold) << std::endl;
            return;
        }

        // La salute della pianta viene aggiornata sia se la pianta cresce sia se la crescita è interrotta
        update_health(temp, hum, light, season);

        // Pianta cresce
        if (status.health >= growth_threshold)
        {
            std::cout << std::format("La pianta {} cresce ({:.2f}% >= {:.2f}%)", name, status.health, growth_threshold) << std::endl;
            apply_growth(temp, hum, light);
        }
        else
        {
            std::cout << std::format("La pianta {} ha interrotto la crescita ({:.2f}% < {:.2f}%)", name, status.health, growth_threshold) << std::endl;
        }
    }

    // Verifica se le condizioni ambientali sono ideali per la pianta
    bool Plant::is_environment_ideal(const float temp, const float hum, const float light) const noexcept
    {
        return (temp >= ideal_parameters.temperature.min && temp <= ideal_parameters.temperature.max &&
                hum >= ideal_parameters.humidity.min && hum <= ideal_parameters.humidity.max &&
                light >= ideal_parameters.light.min && light <= ideal_parameters.light.max);
    }

    // Calcola i fattori ambientali che influenzano la crescita della pianta
    float Plant::calculate_environment_factor(const float current_value, const float min_value, const float max_value, const GrowthFactors& factors) noexcept
    {
        return current_value >= min_value && current_value <= max_value ? factors.ideal :
               current_value > max_value ? factors.above_max : factors.below_min;
    }

    GrowthStage Plant::get_growth_stage() const noexcept // const perchè non modifica lo stato
    {
        return status.health <= death_threshold ? GrowthStage::Dead :
               status.growth <= 2.0f ? GrowthStage::Bud :
               status.growth <= 6.0f ? GrowthStage::Seedling : GrowthStage::Adult;
    }

    std::string Plant::get_growth_stage_str() const
    {
        switch (get_growth_stage())
        {
            case GrowthStage::Bud:
                return "Bud";

            case GrowthStage::Seedling:
                return "Seedling";

            case GrowthStage::Adult:
                return "Adult";

            case GrowthStage::Dead:
                return "Dead";

            default:
                return "Unknown";
        }
    }

    std::string Plant::get_plant_name() const
    {
        return name;
    }

    const PlantStatus& Plant::get_plant_status() const noexcept
    {
        return status;
    }

    std::string Plant::printPlant() const
    {
        return std::format(
            "Plant: {}\n"
            "Growth Threshold: {:.2f} - Death Threshold: {:.2f}\n"
            "Ideal temperature: {:.2f} - {:.2f}\n"
            "Ideal humidity: {:.2f} - {:.2f}\n"
            "Ideal light: {:.2f} - {:.2f}\n"
            "Status ({}): {:.2f} (growth) - {:.2f} (health)\n",
            name,
            growth_threshold,                 death_threshold,
            ideal_parameters.temperature.min, ideal_parameters.temperature.max,
            ideal_parameters.humidity.min,    ideal_parameters.humidity.max,
            ideal_parameters.light.min,       ideal_parameters.light.max,
            get_growth_stage_str(),
            status.growth,
            status.health
        );
    }
}
