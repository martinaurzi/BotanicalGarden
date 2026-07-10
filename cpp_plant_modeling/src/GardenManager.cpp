#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "Types.h"
#include "DesertPlant.h"
#include "TropicalPlant.h"
#include "TemperatePlant.h"
#include "GardenManager.h"
#include "utils.h"

namespace BotanicalGarden
{
    GardenManager& GardenManager::get_instance()
    {
        // instance viene creato solo la prima volta che questa funzione viene chiamata. Successivamente verrà restituita sempre quell'istanza
        static GardenManager instance;

        return instance;
    }

    void GardenManager::load_garden_from_file(const std::string& filename)
    {
        // Apriamo il file in lettura
        std::fstream file(filename, std::ios::in);

        if (!file.is_open())
        {
            throw std::runtime_error("Non e' stato possibile aprire il file " + filename);
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);

            std::string name, plant_type; // plant_type = Desert/Tropical/Temperate
            IdealEnvironment env{};
            std::unique_ptr<Plant> plant_ptr = nullptr;
            std::string label;

            try
            {
                // Leggiamo i parametri comuni a tutti i tipi di piante
                ss >> plant_type >> name >> env.temperature.min >> env.temperature.max >> env.humidity.min >> env.humidity.max >> env.light.min >> env.light.max;

                if (ss.fail())
                {
                    throw std::invalid_argument("Errore nella lettura dei parametri. Parametri incompleti o corrotti.");
                }

                if (plant_type == "Desert")
                {
                    plant_ptr = std::make_unique<DesertPlant>(name, env);
                }
                else if (plant_type == "Tropical")
                {
                    // Creiamo il puntatore a TropicalPlant considerando per direct_light_tolerance e thermal_shock_vulnerability i valori di default. Questo perchè potrebbero non essere presenti nel file
                    // E' stato definito questo puntatore specifico di tipo TropicalPlant per accedere ai metodi setter della classe TropicalPlant.
                    auto tropical_plant_ptr = std::make_unique<TropicalPlant>(name, env);

                    // Controlliamo se direct_light_tolerance e/o thermal_shock_vulnerability sono presenti usando delle label ed eventualmente modifichiamo i valori di default usando i setter
                    while (ss >> label)
                    {
                        // Direct Light Tolerance
                        if (label == "DLIGHT")
                        {
                            if (float direct_light_tol; ss >> direct_light_tol)
                            {
                                tropical_plant_ptr->set_direct_light_tolerance(direct_light_tol);
                            }
                        }
                        else if (label == "TSHOCK")
                        {
                            if (float thermal_shock_vul; ss >> thermal_shock_vul)
                            {
                                tropical_plant_ptr->set_thermal_shock_vulnerability(thermal_shock_vul);
                            }
                        }
                    }

                    plant_ptr = std::move(tropical_plant_ptr);
                }
                else if (plant_type == "Temperate")
                {
                    float cold_dorm, low_light_tol;

                    ss >> cold_dorm >> low_light_tol;

                    auto temperate_plant_ptr = std::make_unique<TemperatePlant>(name, env, cold_dorm, low_light_tol);

                    while (ss >> label)
                    {
                        if (label == "LEAF")
                        {
                            if (float leaf_r; ss >> leaf_r)
                            {
                                temperate_plant_ptr->set_leaf_renewal(leaf_r);
                            }
                        }
                    }

                    plant_ptr = std::move(temperate_plant_ptr);
                }

                // Inseriamo il puntatore nel vettore
                if (plant_ptr != nullptr)
                {
                    // Move semantics: permette di trasferire la proprietà di una risorsa da un oggetto all'altro senza copiare la risorsa stessa
                    garden.push_back(std::move(plant_ptr));
                }
            }
            catch (const std::invalid_argument& e)
            {
                throw std::invalid_argument(std::format("Errore nella riga {} del file: {}", line, e.what()));
            }
        }

        file.close();
    }

    bool GardenManager::load_garden()
    {
        constexpr auto PLANT_FILE = "../plants.txt";

        try
        {
            load_garden_from_file(PLANT_FILE);

            if (garden.empty())
            {
                throw std::runtime_error("Il giardino è vuoto");
            }

            std::cout << std::format("Il giardino contiene {} piante\n", garden.size());

            garden_json_serialized = build_garden_json();

            return true;
        }
        catch (const std::invalid_argument& e)
        {
            throw;
        }
        catch (const std::runtime_error& e)
        {
            throw;
        }
        catch (const std::exception& e)
        {
            throw;
        }
    }

    // A partire dal vettore garden costruisce il JSON serializzato da mandare a Go
    std::string GardenManager::build_garden_json() const
    {
        nlohmann::json builded_json_garden = nlohmann::json::array({});

        for (int i = 1; const auto& plant_ptr : garden)
        {
            if (plant_ptr != nullptr){
                nlohmann::json plant_json;

                plant_json["id"] = i;
                plant_json["type"] = plant_ptr->get_plant_type();
                plant_json["name"] = plant_ptr->get_plant_name();
                plant_json["health"] = plant_ptr->get_plant_status().health;
                plant_json["growth"] = plant_ptr->get_plant_status().growth;
                plant_json["growth_stage"] = plant_ptr->get_growth_stage_str();
                plant_json["is_dead"] = plant_ptr->is_dead();

                builded_json_garden.push_back(plant_json);
            }

            i++;
        }

        return builded_json_garden.dump();
    }

    std::string GardenManager::get_season_name(const Season season)
    {
        return season_to_string(season);
    }

    // Applica i cambiamenti dell'ambiente a tutte le piante presenti nel giardino
    std::string GardenManager::apply_environment_changes(float current_temp, float current_hum, float current_light, const Season season)
    {
        if (garden.empty())
        {
            std::cerr << "[GARDEN MANAGER]: Non e' possibile applicare i cambiamenti dell'ambiente se il giardino e' vuoto." << std::endl;
            garden_json_serialized = "[]";

            return garden_json_serialized;
        }

        std::cout << std::format("***STATO DELL'AMBIENTE***: T={:.2f}, H={:.2f}%, L={:.2f}%, Stagione={}\n\n", current_temp, current_hum, current_light, get_season_name(season));

        for (size_t i = 1; const auto& plant_ptr : garden)
        {
            if (plant_ptr != nullptr)
            {
                std::cout << std::format("{}) PIANTA {}", i, plant_ptr->get_plant_name()) << std::endl;

                plant_ptr->update_plant_status(current_temp, current_hum, current_light, season);
                std::cout << plant_ptr->printPlant() << std::endl;
            }
            i++;
        }

        garden_json_serialized = build_garden_json();

        return garden_json_serialized;
    }

    // Restituisce il JSON serializzato rappresentante il garden
    std::string GardenManager::get_garden() const noexcept
    {
        return garden_json_serialized;
    }
}