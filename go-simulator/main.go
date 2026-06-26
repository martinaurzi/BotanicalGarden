package main

import (
	"fmt"
	"log"
	"time"

	"go-simulator/pkg/environment"
	"go-simulator/pkg/models"
	"go-simulator/pkg/simulation"
	"go-simulator/pkg/storage"
)

func main() {

	// inizializzazione database
	db, err := storage.InitDB("plants.db")
	if err != nil {
		log.Fatalf("Errore fatale inizializzazione DB: %v", err)
	}
	fmt.Println("Database inizializzato.")

	// Configurazione ambiente
	cfg := environment.EnvironmentConfig{
		DayTemp:              28.0,
		NightTemp:            18.0,
		DayLight:             80.0,
		NightLight:           10.0,
		BaseHumidity:         60.0,
		ClimateVariationFreq: 2 * time.Second,
		DayDuration:          7 * time.Second,
	}

	// Creazione ambiente
	env := environment.NewEnvironment(cfg)

	// Canale per inviare lo stato dell’ambiente alle piante
	envChan := make(chan models.EnvironmentState)

	//Recuperiamo la lista di piante da C++
	plants := simulation.InitPlants()
	fmt.Printf("Ricevute %d piante iniziali dal motore di simulazione.\n", len(plants))

	initialEnv := env.GetState()
	err = storage.saveSnapshot(db, plants, initialEnv)
	if err != nil {
		log.Printf("Errore salvataggio snapshot iniziale: %v", err) //vedere se conviene fmt o log
	} else {
		fmt.Println("Primo snapshot iniziale salvato nel DB con successo.")
	}

	// Avvio delle goroutine dell’ambiente
	env.Start(cfg, envChan)

	fmt.Println("Simulazione avviata. Premi CTRL+C per fermare.")

	// Ogni volta che cambia l'ambiente inviamo lo stato dell'ambiente a C++ che mi restituisce le piante aggiornate
	for envState := range envChan {
		fmt.Printf("[%s] Cambiamento Ambiente -> Temp: %.2f°C, Luce: %.2f\n, Hum: %.2f\n",
			envState.Timestamp.Format("15:04:05"), envState.Temperature, envState.Light, envState.Humidity)

		plants = simulation.UpdatePlants(envState)

		err := storage.SaveSnapshot(db, plants, envState)
		if err != nil {
			log.Printf("Errore salvataggio snapshot a DB: %v", err)
		}
	}
}
