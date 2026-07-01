#include <format>
#include <iostream>
#include <vector>
#include <memory>

#include "Plant.h"
#include "GardenLoader.h"

using namespace BotanicalGarden;

int main()
{
    constexpr auto PLANT_FILE = "../plants.txt";

    const std::vector<std::unique_ptr<Plant>> garden = load_garden_from_file(PLANT_FILE);

    if (garden.empty())
    {
        std::cerr << "[ERRORE]: Il giardino e' vuoto\n";
        return 1;
    }

    std::cout << std::format("Il giardino contiene {} piante\n", garden.size());

    std::cout << "***INIZIO DELLA SIMULAZIONE***\n\n";

    // Qua devo prima ricevere i valori da go e dopo faccio il ciclo for
    //...

    float current_temp{10.0f};
    float current_hum{18.0f};
    float current_light{20.0f};

    std::cout << std::format("***STATO DELL'AMBIENTE***: T={:.2f}, H={:.2f}%, L={:.2f}%\n\n", current_temp, current_hum, current_light);

    for (size_t i = 1; const auto& plant_ptr: garden)
    {
        if (plant_ptr != nullptr)
        {
            std::cout << std::format("{}) PIANTA {}", i, plant_ptr->get_plant_name()) << std::endl;
            std::cout << "Stato iniziale:\n";
            std::cout << plant_ptr->printPlant() << std::endl;

            plant_ptr->update_plant_status(current_temp, current_hum, current_light);
            std::cout << plant_ptr->printPlant() << std::endl;
        }
        i++;
    }

    return 0;
}
