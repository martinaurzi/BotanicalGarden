package main

import (
    "fmt"
    "time"

    "go-simulator/pkg/environment"
    "go-simulator/pkg/models"
    "go-simulator/pkg/plant"
    "go-simulator/pkg/simulation"
    "go-simulator/pkg/storage"
)

func main() {

    // Configurazione ambiente
    cfg := environment.EnvironmentConfig{
        DayTemp:             25,
        NightTemp:           18,
        DayLight:            80,
        NightLight:          10,
        BaseHumidity:        50,
        ClimateVariationFreq: 2 * time.Second,
        DayDuration:         5 * time.Second,
        NightDuration:       5 * time.Second,
    }

    // Creazione ambiente
    env := environment.NewEnvironment(cfg)

    // Canale per inviare lo stato dell’ambiente alle piante
    envChan := make(chan models.EnvironmentState)

    // Avvio delle goroutine dell’ambiente
    env.Start(cfg, envChan)

    // Creazione Piante
    plants := []*plant.Plant{
        {State: models.PlantState{ID: 1, Species: "rosa", Health: 80, Growth: 0, Stage: "germoglio"}},
        {State: models.PlantState{ID: 2, Species: "girasole", Health: 90, Growth: 1, Stage: "germoglio"}},
    }

    // Canale per inviare snapshot al database
    snapshotChan := make(chan models.PlantState)

    // Avvio delle goroutine piante
    for _, p := range plants {
        simulation.StartPlantRoutine(p, envChan, snapshotChan)
    }

    // inizializzazione database
    db, err := storage.InitDB("plants.db")
    if err != nil {
        panic(err)
    }
    fmt.Println("Database inizializzato.")

    // goroutine writer del database
    go func() {
        for snap := range snapshotChan {
            envState := env.GetState() // prendo lo stato attuale dell’ambiente
            err := storage.InsertSnapshot(db, snap, envState)
            if err != nil {
                fmt.Println("Errore DB:", err)
            } else {
                fmt.Printf("Snapshot salvato: pianta %d, salute %d, crescita %d\n",
                    snap.ID, snap.Health, snap.Growth)
            }
        }
    }()

    fmt.Println("Simulazione avviata. Premi CTRL+C per fermare.")
    select {} // blocca il main per sempre
}