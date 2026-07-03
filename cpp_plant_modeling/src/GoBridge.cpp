#include <format>
#include <cstring>

#include "GardenManager.h"

extern "C" {
    bool init_garden()
    {
        return BotanicalGarden::GardenManager::get_instance().load_garden();
    }

    const char* get_garden()
    {
        const std::string garden_str = BotanicalGarden::GardenManager::get_instance().get_garden();

        return strdup(garden_str.c_str());
    }

    const char* apply_environment_changes(const float temp, const float hum, const float light, BotanicalGarden::Season season)
    {
        const std::string updated_garden_str = BotanicalGarden::GardenManager::get_instance().apply_environment_changes(temp, hum, light, season);

        return strdup(updated_garden_str.c_str());
    }
}