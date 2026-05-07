#include "Plant.h"

#include <iostream>

namespace BotanicalGarden
{
    // Implementazione del costruttore usando MIL. const&: passo per riferimento per evitare la copia
    Plant::Plant(const std::string& n, const IdealEnvironment& ideal, const float growth_th, const float death_th,
        const float growth_r) :
        name(n), ideal_parameters(ideal), growth_threshold(growth_th), death_threshold(death_th),
        status{0.0f, 80.0f}, growth_rate(growth_r){}

    /* Nella funzione update_growth viene implementata la logica di crescita comune a tutte le piante.
     * In particolare, prima di aggiornare la crescita si verifica la salute della pianta.
     *
     * Se la salute della pianta si mantiene sopra una determinata soglia, la pianta cresce.
     * Quando la salute si trova in uno stato intermedio, la pianta interrompe la crescita.
     * Se lo stato di salute si trova sotto la soglia critica, la pianta muore.*/
    void Plant::update_plant_status(float temp, float hum, float light)
    {
        // Pianta morta
        if (status.health <= death_threshold)
        {
            std::cout << "La pianta " << name << " e' morta (salute: " << status.health << ")" << std::endl;
            return; // la pianta è morta quindi devo uscire
        }

        // Pianta cresce
        if (status.health >= growth_threshold)
        {
            std::cout << "La pianta " << name << " cresce (" << status.health << " >= " << growth_threshold << ")" << std::endl;
            apply_growth(temp, hum, light);
        }
        else
        {
            // Pianta interrompe la crescita
            std::cout << "La pianta " << name << " ha interrotto la crescita" << std::endl;
            update_health(temp, hum, light);
        }
    }

    GrowthStage Plant::get_growth_stage() const // const perchè non modifica lo stato
    {
        return (status.health <= death_threshold) ? GrowthStage::Dead :
               (status.growth <= 2.0f) ? GrowthStage::Bud :
               (status.growth <= 6.0f) ? GrowthStage::Seedling : GrowthStage::Adult;
    }

    std::string Plant::get_growth_stage_str() const
    {
        //manca dead: return (get_growth_stage() == GrowthStage::Bud) ? "Bud" : (get_growth_stage() == GrowthStage::Seedling) ? "Seedling" : "Adult";
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

    std::string Plant::getPlantName() const
    {
        return name;
    }

    // Forse meglio fare due get separati per health e per growth?
    PlantStatus Plant::getPlantStatus() const
    {
        return status;
    }

    std::string Plant::toString() const
    {
        return "Plant: " + name + "\n" +
                "Ideal temperature: " + std::to_string(ideal_parameters.temperature.min) + " - " +
                std::to_string(ideal_parameters.temperature.max) + "\n" +
                "Ideal humidity: " + std::to_string(ideal_parameters.humidity.min) + " - " +
                std::to_string(ideal_parameters.humidity.max) + "\n" +
                "Ideal light: " + std::to_string(ideal_parameters.light.min) + " - " +
                std::to_string(ideal_parameters.light.max) + "\n" +
                "Status (" + get_growth_stage_str() + "): " + std::to_string(status.growth) + " (growth) - " + std::to_string(status.health) + " (health)\n";
    }
}
