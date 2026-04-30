#include <iostream>
#include <vector>
#include <memory>

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

    garden.push_back(std::make_unique<DesertPlant>("Cactus", cactus_env,0.9f));

    for (const auto& plant: garden)
    {
        std::cout << plant->toString() << std::endl;
    }

    return 0;
}
