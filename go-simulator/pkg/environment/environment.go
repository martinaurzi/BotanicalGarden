package environment

import (
	"go-simulator/pkg/models"
	"math/rand"
	"sync"
	"time"
)

// Configurazione ambiente
type EnvironmentConfig struct {
	DayTemp              float64 // temperatura alta
	NightTemp            float64 // temperatura bassa
	DayLight             float64 // luce alta
	NightLight           float64 // minor luce
	BaseHumidity         float64 // umidità
	ClimateVariationFreq time.Duration
	DayDuration          time.Duration
}

type Environment struct {
	mutex sync.Mutex
	state models.EnvironmentState
}

func (e *Environment) GetState() models.EnvironmentState {
	e.mutex.Lock()
	defer e.mutex.Unlock()
	return e.state
}

// Avvio della simulazione dell'ambiente
func (e *Environment) Start(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState) {
	go e.dayNightCycle(cfg, envChan)
	go e.climateVariations(cfg, envChan)
}

func NewEnvironment(cfg EnvironmentConfig) *Environment {
	return &Environment{
		state: models.EnvironmentState{
			Temperature: cfg.DayTemp,
			Humidity:    cfg.BaseHumidity,
			Light:       cfg.DayLight,
			Timestamp:   time.Now(),
		},
	}
}

// updateClimate applica variazioni relative (Risolve il problema Read-Modify-Write)
func (e *Environment) updateClimate(deltaTemp, deltaHum float64, envChan chan<- models.EnvironmentState) {
	e.mutex.Lock()
	defer e.mutex.Unlock()

	// Modifichiamo lo stato internamente in modo atomico
	e.state.Temperature += deltaTemp
	e.state.Humidity += deltaHum

	// Bound check per l'umidità
	if e.state.Humidity < 0 {
		e.state.Humidity = 0
	} else if e.state.Humidity > 100 {
		e.state.Humidity = 100
	}

	e.state.Timestamp = time.Now()

	// Inviamo lo stato modificato sul canale
	envChan <- e.state
}

// updateDayNight imposta i valori assoluti di luce e temperatura di base
func (e *Environment) updateDayNight(temp, light float64, envChan chan<- models.EnvironmentState) {
	e.mutex.Lock()
	defer e.mutex.Unlock()

	e.state.Temperature = temp
	e.state.Light = light
	e.state.Timestamp = time.Now()

	envChan <- e.state
}

// Ciclo giorno/notte OTTIMIZZATO per durate uguali
func (e *Environment) dayNightCycle(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState) {
	// Usiamo la durata del giorno come intervallo fisso del metronomo
	ticker := time.NewTicker(cfg.DayDuration)
	defer ticker.Stop()

	isDay := true

	for range ticker.C {
		if isDay {
			e.updateDayNight(cfg.DayTemp, cfg.DayLight, envChan)
		} else {
			e.updateDayNight(cfg.NightTemp, cfg.NightLight, envChan)
		}

		// Inverte lo stato per il prossimo "battito" del ticker
		isDay = !isDay
	}
}

// Variazioni periodiche (Qui il Ticker è perfetto)
func (e *Environment) climateVariations(cfg EnvironmentConfig, envChan chan<- models.EnvironmentState) {
	ticker := time.NewTicker(cfg.ClimateVariationFreq)
	defer ticker.Stop() // Pulizia della risorsa quando la funzione esce

	for range ticker.C {
		// Generazione delta [-2, +2) e [-5, +5)
		deltaTemp := rand.Float64()*4 - 2
		deltaHum := rand.Float64()*10 - 5

		// Applichiamo la variazione atomica
		e.updateClimate(deltaTemp, deltaHum, envChan)
	}
}
