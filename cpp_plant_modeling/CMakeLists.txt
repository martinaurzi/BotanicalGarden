cmake_minimum_required(VERSION 4.0)
project(BotanicalGarden)

set(CMAKE_CXX_STANDARD 20)

set(CPP_SOURCES
        src/Plant.cpp
        src/DesertPlant.cpp
        src/TropicalPlant.cpp
        src/TemperatePlant.cpp
        src/GardenManager.cpp
        src/GoBridge.cpp
)

add_library(BotanicalGardenLib STATIC ${CPP_SOURCES}) # Creiamo una libreria statica a partire dai file sorgenti

target_include_directories(BotanicalGardenLib PUBLIC include) # Indichiamo dove trovare i file .h