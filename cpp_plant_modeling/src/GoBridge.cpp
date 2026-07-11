#include <format>
#include <cstring>
#include <iostream>

#include "GardenManager.h"

extern "C" {
    bool init_garden()
    {
        try
        {
            return BotanicalGarden::GardenManager::get_instance().load_garden();
        }
        catch (const std::invalid_argument& e)
        {
            std::cerr << "[GO BRIDGE]: Invalid Argument: " << e.what() << '\n';
            return false;
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "[GO BRIDGE]: Errore runtime: " << e.what() << '\n';
            return false;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[GO BRIDGE]: Eccezione generica: " << e.what() << '\n';
            return false;
        }
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