#include <string>
#include <cctype>
#include <unordered_map>

#include "Types.h"
#include "utils.h"

namespace BotanicalGarden
{
    bool contains_letters(const std::string &s)
    {
        for (const char c : s)
        {
            if (isalpha(c))
                return true;
        }

        return false;
    }

    std::string growth_stage_to_string(const GrowthStage stage)
    {
        static const std::unordered_map<GrowthStage, std::string> growth_stage_map = {
            {GrowthStage::Bud, "Bud" },
            {GrowthStage::Seedling, "Seedling"},
            {GrowthStage::Adult,"Adult"},
            {GrowthStage::Dead,"Dead"}
        };

        return enum_to_string(stage, growth_stage_map);
    }

    std::string season_to_string(const Season season)
    {
        static const std::unordered_map<Season, std::string> season_map = {
            {Season::Spring, "Spring" },
            {Season::Summer, "Summer"},
            {Season::Autumn,"Autumn"},
            {Season::Winter,"Winter"}
        };

        return enum_to_string(season, season_map);
    }
}
