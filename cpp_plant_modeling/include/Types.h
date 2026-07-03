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

    enum class Season
    {
        Spring,
        Summer,
        Autumn,
        Winter
    };

    enum class GrowthStage
    {
        Bud, // Germoglio
        Seedling, // Piantina
        Adult,
        Dead
    };

    struct GrowthFactors
    {
        float ideal;
        float above_max;
        float below_min;
    };
}
#endif //BOTANICALGARDEN_STRUCTS_H