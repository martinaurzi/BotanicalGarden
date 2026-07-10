package environment

import (
    "math"
    "math/rand"
    "sync"
    "time"
    "log"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
)

type Season int

const (
	Spring Season = iota
	Summer
	Autumn
	Winter

	seasonCount
)

func (s Season) String() string {
    switch s {
    case Spring:
        return "Spring"
    case Summer:
        return "Summer"
    case Autumn:
        return "Autumn"
    case Winter:
        return "Winter"
    default:
        return "Unknown"
    }
}

// Configurazione fissa della simulazione
type SimulationConfig struct {
    DayLight             float64
    NightLight           float64
    ClimateVariationFreq time.Duration
    DayDuration          time.Duration
    SeasonDuration       time.Duration
}

// Valori climatici di riferimento per ogni stagione
type ClimateProfile struct {
    DayTemp          float64
    NightTemp        float64
    BaseHumidity     float64
    HumidityMaxDelta float64
}

//  Modificatori applicati in base al meteo giornaliero (Sole, Nuvole, Pioggia)
type WeatherModifiers struct {
    DeltaTemp float64
    DeltaHum  float64
    DeltaLt   float64
}

// Struttura per gestire lo stato dell'ambiente e i cicli ambientali
type Environment struct {
	mutex          sync.Mutex
	state          models.EnvironmentState
	simCfg         SimulationConfig
    currentProfile ClimateProfile
	currentSeason  Season
    weather        WeatherModifiers
	stopChan       chan struct{}
	stopOnce       sync.Once
	wg             sync.WaitGroup
}

// Profili climatici stagionali predefiniti
var seasonProfiles = map[Season]ClimateProfile{
        // Primavera: Clima mite, umidità media
        Spring: {DayTemp: 24.0, NightTemp: 16.0, BaseHumidity: 45.0, HumidityMaxDelta: 15.0},

        // Estate: Caldo intenso, l'aria è secca di giorno e l'umidità sale poco di notte
        Summer: {DayTemp: 30.0, NightTemp: 22.0, BaseHumidity: 30.0, HumidityMaxDelta: 5.0},

        // Autunno: Fresco e molto umido
        Autumn: {DayTemp: 19.0, NightTemp: 14.0, BaseHumidity: 65.0, HumidityMaxDelta: 15.0},

        // Inverno: Freddo e costantemente umido/bagnato
        Winter: {DayTemp: 12.0, NightTemp: 5.0,  BaseHumidity: 70.0, HumidityMaxDelta: 8.0},
}

// Ritorna una copia dello stato ambientale corrente
func (e *Environment) GetState() models.EnvironmentState {
	e.mutex.Lock()
	defer e.mutex.Unlock()
	return e.state
}

// Inizializza l'ambiente con la configurazione data e lo stato iniziale (Autunno)
func NewEnvironment(cfg SimulationConfig) *Environment {
    env := &Environment{
        simCfg:         cfg,
        currentSeason:  Autumn,
        currentProfile: seasonProfiles[Autumn],
        weather:        WeatherModifiers{DeltaTemp: 2.5, DeltaHum: -5, DeltaLt: 10},
        stopChan:       make(chan struct{}),
    }

    env.state = models.EnvironmentState{
        Temperature: env.currentProfile.DayTemp,
        Humidity:    env.currentProfile.BaseHumidity - env.currentProfile.HumidityMaxDelta,
        Light:       cfg.DayLight,
        Season:      int(env.currentSeason),
        Timestamp:   time.Now(),
    }

    return env
}

// Chiudere il canale stopChan una sola volta per arrestare i cicli di simulazione attivi
func (e *Environment) Stop() {
    e.stopOnce.Do(func() {
            close(e.stopChan)
    })
}

// Attende la terminazione di tutte le goroutine dell'ambiente
func (e *Environment) Wait() {
    e.wg.Wait()
}

// Avvia le goroutine concorrenti per i cicli ambientali
func (e *Environment) Start(envChan chan<- models.EnvironmentState) {
	e.wg.Add(3)

	go e.dayNightCycle(envChan)
	go e.climateVariations()
	go e.seasonCycle()
}

// Calcola e aggiorna i parametri ambientali(luce,temperatura e umidità) in base al tempo trascorso dall'inizio della simulazione
func (e *Environment) updateTimeAndBaseClimate(startTime time.Time, envChan chan<- models.EnvironmentState) {
	// Viene determinato a che punto del ciclo giorno/notte ci troviamo (da 0.0 inizio a 1.0 fine)
	elapsed := time.Since(startTime)
	normalizedTime := math.Mod(elapsed.Seconds(), e.simCfg.DayDuration.Seconds()) / e.simCfg.DayDuration.Seconds()

	// Oscillazione sinusoidale tra 0.0 (notte fonda) e 1.0 (pieno giorno)
	coef := 0.5 * (1.0 + math.Cos(2.0*math.Pi*normalizedTime))

    e.mutex.Lock()

    e.state.Humidity = e.currentProfile.BaseHumidity + e.currentProfile.HumidityMaxDelta*(1.0-2.0*coef) + e.weather.DeltaHum
	e.state.Temperature = e.currentProfile.NightTemp + (e.currentProfile.DayTemp-e.currentProfile.NightTemp)*coef + e.weather.DeltaTemp
	e.state.Light = e.simCfg.NightLight + (e.simCfg.DayLight-e.simCfg.NightLight)*coef + e.weather.DeltaLt

    if e.state.Light < e.simCfg.NightLight { e.state.Light = e.simCfg.NightLight }

	e.state.Season = int(e.currentSeason)
	e.state.Timestamp = time.Now()

	stateCopy := e.state
    e.mutex.Unlock()

	envChan <- stateCopy
}

// Goroutine: aggiorna periodicamente lo stato dell'ambiente simulando l'alternanza giorno/notte.
func (e *Environment) dayNightCycle(envChan chan<- models.EnvironmentState) {
    ticker := time.NewTicker(500 * time.Millisecond)
	defer ticker.Stop()
	defer e.wg.Done()

    // Registriamo l'istante in cui la simulazione è partita
	startTime := time.Now()

    for{
         select{
            case <-e.stopChan:
                log.Println("[INFO] dayNightCycle interrotto.")
                return
            case <-ticker.C:
                e.updateTimeAndBaseClimate(startTime, envChan)
         }
    }
}

// Goroutine: introduce variazioni meteo casuali a intervalli regolari(ogni giorno simulato)
func (e *Environment) climateVariations() {
    ticker := time.NewTicker(e.simCfg.ClimateVariationFreq)
    defer ticker.Stop()
    defer e.wg.Done()

    for {
        select {
        case <-e.stopChan:
            log.Println("[INFO] climateVariations interrotto.")
            return

        case <-ticker.C:
            weather := rand.Intn(10)
            var dT, dH, dL float64
            var climateVariation string

            if weather < 5 { // 50% probabilità: SOLE
                climateVariation = "Sole"
                dT = 2.5
                dH = -5.0
                dL = 10.0
            } else if weather < 8 { // 30% probabilità: NUVOLE
                climateVariation = "Nuvole"
                dT = -1.5
                dH = 5.0
                dL = -5.0
            } else { // 20% probabilità: PIOGGIA
                climateVariation = "Pioggia"
                dT = -3.5
                dH = 10.0
                dL = -15.0
            }

            log.Printf("[METEO] Cambiamento meteo: %s", climateVariation)

            e.mutex.Lock()
            e.weather.DeltaTemp = dT
            e.weather.DeltaHum = dH
            e.weather.DeltaLt = dL
            e.mutex.Unlock()
        }
    }
}

// Goroutine: gestisce l'avanzamento ciclico delle stagioni e aggiorna il profilo climatico
func (e *Environment) seasonCycle() {
	ticker := time.NewTicker(e.simCfg.SeasonDuration)
	defer ticker.Stop()
    defer e.wg.Done()

    for{
        select{
            case <-e.stopChan:
                log.Println("[INFO] seasonCycle interrotto.")
                return

            case <-ticker.C:
                e.mutex.Lock()
                // Passiamo alla stagione successiva
                e.currentSeason = (e.currentSeason + 1) % seasonCount

               // Applicazione del nuovo profilo climatico
               e.currentProfile = seasonProfiles[e.currentSeason]

                log.Printf("[STAGIONE] Nuova stagione avviata: %s!",
                    e.currentSeason.String())
                e.mutex.Unlock()
        }
    }
}