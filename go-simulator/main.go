package main

import (
    "encoding/json"
    "fmt"
    "time"
    "log"
    "unsafe"

    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/environment"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/storage"
)

/*
#cgo CFLAGS: -I../cpp_plant_modeling/include
#cgo LDFLAGS: -L../cpp_plant_modeling/cmake-build-debug -lBotanicalGardenLib -lstdc++

#include <stdlib.h>
#include <stdbool.h>

bool init_garden();
const char* get_garden();
const char* apply_environment_changes(const float temp, const float hum, const float light, const int season);
*/
import "C"

const simulationDuration = 48 * time.Second

// Converte la stringa JSON da C++ in una slice di PlantState
func parsePlantsFromJSON(jsonStr string) ([]models.PlantState, error) {
	var plants []models.PlantState
	err := json.Unmarshal([]byte(jsonStr), &plants)
	return plants, err
}

func main() {
        // Inizializzazione e pulizia del Database
    	db, err := storage.InitDB("plants.db")
    	if err != nil {
    		log.Fatalf("[ERRORE] Errore di inizializzazione DB: %v", err) //log per avere data e ora di quando avviene la stampa, %v per stampare il valore di error nel formato predefinito
    	}
        defer db.Close()

        err = storage.ClearSnapshots(db)
        if err != nil {
            log.Fatalf("[ERRORE] Errore durante la pulizia del database: %v", err) //Fatalf effettua la stampa e poi effettua una chiamata a os.Exit(1)
        }
        fmt.Println("Database inizializzato e tabella plant_snapshots svuotata.")

        // Inizializzazione giardino in C++
        if !bool(C.init_garden()) {
            log.Fatalf("[ERRORE] Impossibile caricare il giardino dal file in C++")
        }
        fmt.Println("Giardino inizializzato con successo in C++.")

        // Recupero dello stato iniziale delle piante dal C++ via Cgo
        cGardenStr := C.get_garden()
        goGardenStr := C.GoString(cGardenStr)

        C.free(unsafe.Pointer(cGardenStr)) // Libera la memoria allocata sul lato C++

        plants, err := parsePlantsFromJSON(goGardenStr)
        if err != nil {
            log.Fatalf("[ERRORE] Errore nel parsing del JSON delle piante iniziali: %v", err)
        }
        fmt.Printf("Ricevute %d piante iniziali da C++.\n", len(plants))

    	// Configurazione simulazione
    	cfg := environment.SimulationConfig{
    		DayLight:             80.0,
    		NightLight:           30.0,
    		ClimateVariationFreq: 6 * time.Second,
    		DayDuration:          6 * time.Second,
    		SeasonDuration:       12 * time.Second,
    	}

    	// Creazione ambiente
    	env := environment.NewEnvironment(cfg)

    	// Canale per notificare al main le modifiche dello stato dell’ambiente
    	envChan := make(chan models.EnvironmentState, 10)

        // Recupero stato iniziale dell'ambiente
    	initialEnv := env.GetState()
    	// Salvataggio stato inziale del sistema (piante e stato ambiente)
    	err = storage.SaveSnapshot(db, plants, initialEnv)
    	if err != nil {
    		log.Printf("[ERRORE] Errore salvataggio snapshot iniziale: %v", err)
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

                // Invio dei nuovi parametri ambientali a C++ per aggiornare lo stato di crescita delle piante
                cUpdatedGardenStr := C.apply_environment_changes(
                    C.float(envState.Temperature),
                    C.float(envState.Humidity),
                    C.float(envState.Light),
                    C.int(envState.Season),
                )
                goUpdatedGardenStr := C.GoString(cUpdatedGardenStr)

                C.free(unsafe.Pointer(cUpdatedGardenStr))

                // Otteniamo lo slice contenente lo stato delle piante aggiornato
                plants, err = parsePlantsFromJSON(goUpdatedGardenStr)
                if err != nil {
                    log.Printf("[ERRORE] Errore nel parsing del JSON aggiornato: %v", err)
                    continue
                }

                // Salvataggio nel database dei nuovi snapshots
                err = storage.SaveSnapshot(db, plants, envState)
                if err != nil {
                    log.Printf("[ERRORE] Errore salvataggio snapshot a DB: %v", err)
                }

            case <-done:
                // Scaduto il tempo di simulazione termina l'esecuzione delle goroutine
                log.Println("\nTempo limite di 36 secondi raggiunto. Arresto della simulazione in corso")
                env.Stop()
                env.Wait()
                fmt.Println("Tutte le goroutine terminate.")
                return
            }
        }
}