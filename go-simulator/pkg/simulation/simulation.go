package simulation

import (
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
)

// Simulazione funzioni cgo

// Piante restituite da C++
var internalPlants []models.PlantState

func InitPlants() []models.PlantState {
	// C++ decide quali e quante piante creare all'avvio
	internalPlants = []models.PlantState{
		{ID: 1, Species: "Rosa", Health: 100, Growth: 0, Stage: "germoglio"},
		{ID: 2, Species: "Cactus", Health: 100, Growth: 0, Stage: "germoglio"},
		{ID: 3, Species: "Pino", Health: 100, Growth: 0, Stage: "germoglio"},
	}

	return internalPlants
}

func UpdatePlants(env models.EnvironmentState) []models.PlantState {
	// Simula la logica di C++ basata sul meteo
	for i := range internalPlants {
		// Se fa troppo caldo o c'è troppa poca luce, la pianta soffre
		if env.Temperature > 30.0 || env.Light < 20.0 {
			if internalPlants[i].Health > 0 {
				internalPlants[i].Health -= 5 // Perde salute
			}
		} else {
			// Condizioni ottimali: la pianta cresce
			if internalPlants[i].Growth < 10 {
				internalPlants[i].Growth += 1
			}
			// Cambiamento di stage basato sulla crescita
			if internalPlants[i].Growth >= 5 && internalPlants[i].Stage == "germoglio" {
				internalPlants[i].Stage = "piantina"
			}
		}
	}

	// Ritorniamo il vettore aggiornato
	return internalPlants
}
