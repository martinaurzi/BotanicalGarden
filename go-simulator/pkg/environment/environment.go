package environment

import (
    "math/rand"
    "sync"
    "time"
    "go-simulator/pkg/models"
)

// Configurazione ambiente
type EnvironmentConfig struct {
    DayTemp float64 // temperatura alta
    NightTemp float64 // temperatura bassa
    DayLight float64 // luce alta
    NightLight float64 // minor luce
    BaseHumidity float64 // umidità
    ClimateVariationFreq time.Duration
    DayDuration time.Duration
    NightDuration time.Duration
}

type Environment struct {
    state models.EnvironmentState
    mutex sync.RWMutex
}

// Avvio della simulazione dell'ambiente
func (e *Environment) Start(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState){
    go e.dayNightCycle(cfg, envChan)
    go e.climateVariations(cfg, envChan)
}

func NewEnvironment(cfg EnvironmentConfig) *Environment {
    return &Environment{
        state: models.EnvironmentState{
            Temperature: cfg.DayTemp,
            Humidity: cfg.BaseHumidity,
            Light: cfg.DayLight,
            Timestamp: time.Now(),
        },
    }
}

// Blocca la lettura dello stato per fornire una copia
func (e *Environment) GetState() models.EnvironmentState {
    e.mutex.RLock()
    defer e.mutex.RUnlock()
    return e.state
}

// Aggiorna lo stato interno dell'ambiente
func (e *Environment) updateState(temperature, humidity, light float64){
    e.mutex.Lock()
    defer e.mutex.Unlock()

    e.state.Temperature = temperature
    e.state.Humidity = humidity
    e.state.Light = light
    e.state.Timestamp = time.Now()
}

// Ciclo giorno/notte
func (e *Environment) dayNightCycle(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState){
    isDay := true

    for {
        if isDay{
            e.updateState(cfg.DayTemp, e.GetState().Humidity, cfg.DayLight)
            envChan <- e.GetState()
            time.Sleep(cfg.DayDuration)
        } else {
            e.updateState(cfg.NightTemp, e.GetState().Humidity, cfg.NightLight)
            envChan <- e.GetState()
            time.Sleep(cfg.NightDuration)
        }
        isDay = !isDay
    }
}

// Variazioni periodiche della temperatura e umidità
func (e Environment) climateVariations(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState){
    rand.Seed(time.Now().UnixNano()) //prendo il tempo attuale in nanosecondi e lo passo come seed, per avere sempre un seed differente

    for {
        time.Sleep(cfg.ClimateVariationFreq)

        current := e.GetState()

        // variazioni casuali controllate rand.Float64() genera un numero casuale tra [0.0 e 1.0)
        // *4 aumenta l'intervallo a [0, 4) con -2 [-2, +2)
        deltaTemp := (rand.Float64()*4 - 2) //[-2, +2]
        deltaHum := (rand.Float64()*10 - 5) //[-5, +5]

        newTemp := current.Temperature + deltaTemp
        newHum := current.Humidity + deltaHum

        if newHum < 0 { // non esiste umidità negativa
            newHum = 0
        }
        if newHum > 100 {
            newHum = 100
        }

        e.updateState(newTemp, newHum, current.Light)
        envChan <- e.GetState()
    }
}