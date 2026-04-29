#include "Plant.h"

namespace BotanicalGarden
{
    /* Implementazione del costruttore usando MIL
     * const&: passo per riferimento per evitare la copia
    */
    Plant::Plant(const std::string& n, const IdealEnvironment& ideal, float growth_r) :
        name(n), ideal_parameters(ideal), status{0, 80}, growth_rate(growth_r){}

    GrowthStage Plant::get_growth_stage() const // const perchè non modifica lo stato
    {
        if (status.growth <= 2)
        {
            return GrowthStage::Bud;
        }

        if (status.growth <= 6)
        {
            return GrowthStage::Seedling;
        }

        return GrowthStage::Adult;

        // Versione con operatore ternario
        // return (status.growth <= 2) ? GrowthStage::Bud : (status.growth <= 6) ? GrowthStage::Seedling : GrowthStage::Adult;
    }
}


