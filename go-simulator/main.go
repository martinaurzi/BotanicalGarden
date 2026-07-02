package main

import (
    "fmt"
    "time"
    "log"

    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/environment"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/simulation"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/storage"
)

const simulationDuration = 36 * time.Second

func main() {

    // inizializzazione database
    	db, err := storage.InitDB("plants.db")
    	if err != nil {
    		log.Fatalf("Errore fatale inizializzazione DB: %v", err)
    	}

        defer db.Close()

        err = storage.ClearSnapshots(db)
        if err != nil {
            log.Fatalf("Errore durante la pulizia del database: %v", err)
        }

        fmt.Println("Database inizializzato e tabella snapshot svuotata.")

    	// Configurazione simulazione
    	cfg := environment.SimulationConfig{
    		DayLight:             100.0,
    		NightLight:           10.0,
    		BaseHumidity:         50.0,
    		ClimateVariationFreq: 2 * time.Second,
    		DayDuration:          6 * time.Second,
    		SeasonDuration:       18 * time.Second,
    	}

    	// Creazione ambiente
    	env := environment.NewEnvironment(cfg)

    	// Canale per inviare lo stato dell’ambiente alle piante, con buffer di 100 messaggi
    	envChan := make(chan models.EnvironmentState, 100)

    	//Recuperiamo la lista di piante da C++
    	plants := simulation.InitPlants()
    	fmt.Printf("Ricevute %d piante iniziali dal motore di simulazione.\n", len(plants))

    	initialEnv := env.GetState()
    	err = storage.SaveSnapshot(db, plants, initialEnv)
    	if err != nil {
    		log.Printf("Errore salvataggio snapshot iniziale: %v", err) //vedere se conviene fmt o log
    	} else {
    		fmt.Println("Primo snapshot iniziale salvato nel DB con successo.")
    	}

    	// Avvio delle goroutine dell’ambiente
    	env.Start(envChan)

    	fmt.Println("Simulazione avviata. Terminerà automaticamente tra 36 secondi")

    	done := time.After(simulationDuration)


        for {
            select {
            case envState := <-envChan:
                fmt.Printf("[%s] Cambiamento Ambiente -> Temp: %.2f°C, Luce: %.2f, Hum: %.2f\n",
                    envState.Timestamp.Format("15:04:05"), envState.Temperature, envState.Light, envState.Humidity)

                plants = simulation.UpdatePlants(envState)

                err := storage.SaveSnapshot(db, plants, envState)
                if err != nil {
                    log.Printf("Errore salvataggio snapshot a DB: %v", err)
                }

            case <-done:
                // Scaduti i 36 secondi, usciamo dal ciclo infinito
                fmt.Println("\nTempo limite di 36 secondi raggiunto. Arresto della simulazione in corso")
                env.Stop()
                env.Wait()
                fmt.Println("Tutte le goroutine terminate.")
                return // serve la label perchè break da solo esce solo da select, mentre break Loop esce dal for esterno
            }
        }
}