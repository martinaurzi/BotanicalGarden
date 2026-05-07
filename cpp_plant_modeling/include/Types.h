#ifndef BOTANICALGARDEN_STRUCTS_H
#define BOTANICALGARDEN_STRUCTS_H

namespace BotanicalGarden
{
    struct PlantStatus
    {
        float growth; // 0-10
        float health; // 0-100
    };

    struct Range
    {
        float min;
        float max;
    };

    struct IdealEnvironment
    {
        Range temperature;
        Range humidity;
        Range light;
    };

    enum class GrowthStage
    {
        Bud, // Germoglio
        Seedling, // Piantina
        Adult,
        Dead
    };
}
#endif //BOTANICALGARDEN_STRUCTS_H