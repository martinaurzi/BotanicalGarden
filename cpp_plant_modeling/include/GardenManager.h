#ifndef BOTANICALGARDEN_GARDENMANAGER_H
#define BOTANICALGARDEN_GARDENMANAGER_H

#include <memory>
#include <vector>

#include "Plant.h"

namespace BotanicalGarden
{
    class GardenManager
    {
        std::vector<std::unique_ptr<Plant>> garden{};
        std::string garden_json_serialized;

        // Costruttore privato in modo che non si possano creare nuove istanze della classe dell'esterno (Singleton)
        // = default per generare implementazione di default del costruttore
        GardenManager() = default;

        std::string build_garden_json() const;

    public:
        static GardenManager& get_instance();

        // = delete impedisce la chiamata/istanziazione di una funzione producendo un errore a compile-time
        GardenManager(const GardenManager&) = delete;
        GardenManager& operator=(const GardenManager&) = delete;

        void load_garden_from_file(const std::string& filename);
        bool load_garden();

        std::string get_garden() const;

        // devo passare tutto quello che mi passa go. in caso vedere se fare tornare std::string oppure poi farlo dopo quando devo mandare a go
        std::string apply_environment_changes(float current_temp, float current_hum, float current_light);
    };
}
#endif //BOTANICALGARDEN_GARDENMANAGER_H