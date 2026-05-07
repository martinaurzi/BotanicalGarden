#include <iostream>
#include <vector>
#include <memory>
#include <thread>

#include "Plant.h"
#include "DesertPlant.h"
#include "TropicalPlant.h"
#include "TemperatePlant.h"

using namespace BotanicalGarden;

int main()
{
    // Creo un vettore di smart pointers di tipo Plant
    std::vector<std::unique_ptr<Plant>> garden;

    IdealEnvironment cactus_env{
        {20.0f, 45.0f},
        {10.0f, 30.0f},
        {80.0f, 100.0f}
    };

    garden.push_back(std::make_unique<DesertPlant>("Cactus", cactus_env));

    for (const auto& plant: garden)
    {
        std::cout << plant->toString() << std::endl;
        plant->update_plant_status(30.0f, 28.5f, 85.0f); // (0.4 + 0.3 + 0.5) * 0.3 = 0,36 ; water_reserve=0.2; health = 85
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(30.0f, 35.0f, 85.0f); // (0.4 + 0 + 0.5) * 0.3 = 0,27 ; water_reserve=2.2 ; health -= 2 * (35-30) * 0.1 = 84
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(30.0f, 40.0f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(60.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        plant->update_plant_status(30.0f, 28.5f, 85.0f);
        std::cout << plant->toString() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        break;
    }

    return 0;
}
