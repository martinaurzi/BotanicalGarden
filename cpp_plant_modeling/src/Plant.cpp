#include "Plant.h"

namespace BotanicalGarden
{
    // Implementazione del costruttore usando MIL. const&: passo per riferimento per evitare la copia
    Plant::Plant(const std::string& n, const IdealEnvironment& ideal, const float growth_r) :
        name(n), ideal_parameters(ideal), status{0, 80}, growth_rate(growth_r){}

    GrowthStage Plant::get_growth_stage() const // const perchè non modifica lo stato
    {
        return (status.growth <= 2) ? GrowthStage::Bud : (status.growth <= 6) ? GrowthStage::Seedling : GrowthStage::Adult;
    }

    std::string Plant::get_growth_stage_str() const
    {
        //return (get_growth_stage() == GrowthStage::Bud) ? "Bud" : (get_growth_stage() == GrowthStage::Seedling) ? "Seedling" : "Adult";
        switch (get_growth_stage())
        {
            case GrowthStage::Bud:
                return "Bud";

            case GrowthStage::Seedling:
                return "Seedling";

            case GrowthStage::Adult:
                return "Adult";

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


