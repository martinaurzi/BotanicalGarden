cmake_minimum_required(VERSION 4.0)
project(BotanicalGarden)

set(CMAKE_CXX_STANDARD 20)

add_executable(BotanicalGarden
        src/main.cpp
        src/Plant.cpp
        src/DesertPlant.cpp
        src/TropicalPlant.cpp
        src/TemperatePlant.cpp
        src/GardenManager.cpp
        src/GardenManager.h
        src/GoBridge.cpp
)

target_include_directories(BotanicalGarden PUBLIC include)