#include "GardenManager.h"

extern "C" {
    bool init_garden()
    {
        return BotanicalGarden::GardenManager::get_instance().load_garden();
    }

    const char* get_garden()
    {
        // .c_str() per convertire std::string di C++ in una stringa C
        return BotanicalGarden::GardenManager::get_instance().get_garden().c_str();
    }

    const char* apply_environment_changes(const float temp, const float hum, const float light)
    {
        return BotanicalGarden::GardenManager::get_instance().apply_environment_changes(temp, hum, light).c_str();
    }
}