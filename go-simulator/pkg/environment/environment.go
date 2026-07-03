package environment

import (
    "math"
    "math/rand"
    "sync"
    "time"
    "log"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
)

type Season int // enum non esistono in go

const (
	Spring Season = iota //Spring Season = iota si puo fare anche solo così sulla prima riga
	Summer
	Autumn
	Winter

	seasonCount //inizializzato a 4 da iota
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
    BaseHumidity         float64
    ClimateVariationFreq time.Duration
    DayDuration          time.Duration
    SeasonDuration       time.Duration
}

// Profilo climatico stagionale (valori di riferimento per ogni stagione)
type ClimateProfile struct {
    DayTemp          float64
    NightTemp        float64
    HumidityMaxDelta float64
}

type Environment struct {
	mutex sync.Mutex
	state models.EnvironmentState
	simCfg SimulationConfig   // Sola lettura: non serve mutex per leggerla
    currentProfile ClimateProfile     // Mutabile va protetta con mutex
	currentSeason Season
	stopChan chan struct{} //canale per fermare i ticker
	stopOnce sync.Once //facciamo in modo che la funzione Stop possa essere chiamata una sola volta se no genera panic
	wg sync.WaitGroup
}

// Definiamo i profili climatici per ogni stagione
var seasonProfiles = map[Season]ClimateProfile{ // definizione di una mappa(dictionary) che associa ad ogni stagione il profilo climatico (key: value)
    Spring: {DayTemp: 22.0, NightTemp: 12.0, HumidityMaxDelta: 15.0}, // go permette di omettere ClimateProfile{} perché lo inferisce automaticamente
    Summer: {DayTemp: 32.0, NightTemp: 20.0, HumidityMaxDelta: 25.0},
    Autumn: {DayTemp: 18.0, NightTemp: 8.0,  HumidityMaxDelta: 10.0},
    Winter: {DayTemp: 10.0, NightTemp: 0.0,  HumidityMaxDelta: 5.0},
}

func (e *Environment) GetState() models.EnvironmentState {
	e.mutex.Lock()
	defer e.mutex.Unlock()
	return e.state //ritorna una copia perchè la struct è value type
}

func NewEnvironment(cfg SimulationConfig) *Environment {
    // Creiamo l'istanza di Environment e iniziamo dalla primavera
    env := &Environment{
        simCfg:        cfg,
        currentSeason: Spring,
        currentProfile: seasonProfiles[Spring],
        stopChan:      make(chan struct{}), //inizializzazione del canale di Stop
    }

    // Inizializziamo lo stato iniziale
    env.state = models.EnvironmentState{
        Temperature: env.currentProfile.NightTemp,
        Humidity:    cfg.BaseHumidity,
        Light:       cfg.NightLight,
        Season:      int(env.currentSeason),
        Timestamp:   time.Now(),
    }

    return env
}

func (e *Environment) Stop() {
    e.stopOnce.Do(func() { //funzione anonima perchè non posso passare direttamente close()
            close(e.stopChan)
    })
}

func (e *Environment) Wait() {
    e.wg.Wait()
}

// Avvio della simulazione dell'ambiente
func (e *Environment) Start(envChan chan<- models.EnvironmentState) {
	e.wg.Add(3)

	go e.dayNightCycle(envChan)
	go e.climateVariations(envChan)
	go e.seasonCycle()
}

// Aggiorna i valori di luce e temperatura in base al tempo trascorso
func (e *Environment) updateTimeAndBaseClimate(startTime time.Time, envChan chan<- models.EnvironmentState) {
	// Calcoliamo a che punto del ciclo ci troviamo (da 0.0 inizio a 1.0 fine)
	elapsed := time.Since(startTime)
	normalizedTime := math.Mod(elapsed.Seconds(), e.simCfg.DayDuration.Seconds()) / e.simCfg.DayDuration.Seconds()

	// Usiamo il coseno per creare un'oscillazione del tra 0.0 e 1.0
	// Sfasiamo di pi greco per far partire la simulazione con coefficiente = 0.0
	coef := 0.5 * (1.0 + math.Cos(2.0*math.Pi*normalizedTime - math.Pi))

    e.mutex.Lock()
	// Quando coef è 0 (notte) -> umidità è BaseHumidity + MaxDelta (es. 60 + 20 = 80%)
    // Quando coef è 1 (giorno) -> umidità è BaseHumidity - MaxDelta (es. 60 - 20 = 40%)
    e.state.Humidity = e.simCfg.BaseHumidity + e.currentProfile.HumidityMaxDelta*(1.0-2.0*coef) //comportamento opposto alla temperatura

	e.state.Temperature = e.currentProfile.NightTemp + (e.currentProfile.DayTemp-e.currentProfile.NightTemp)*coef
	e.state.Light = e.simCfg.NightLight + (e.simCfg.DayLight-e.simCfg.NightLight)*coef

	e.state.Season = int(e.currentSeason)

	e.state.Timestamp = time.Now()

	stateCopy := e.state

    e.mutex.Unlock()

	envChan <- stateCopy
}

func (e *Environment) dayNightCycle(envChan chan<- models.EnvironmentState) {
    // Creiamo un Ticker che genera un tick ogni 500 millisecondi
    ticker := time.NewTicker(500 * time.Millisecond)
	defer ticker.Stop()
	defer e.wg.Done()

    // Registriamo il momento esatto in cui la simulazione è partita
	startTime := time.Now()

    // Loop infinito: ogni 500ms il ticker si attiva ed esegue il calcolo
    for{
         select{ // select esegue una sola scelta e poi termina (serve solo per i canali, gestire eventi concorrenti)
            case <-e.stopChan:
                log.Println("[INFO] dayNightCycle interrotto.")
                return //esce dalla funzione attivando defer
            case <-ticker.C:
                e.updateTimeAndBaseClimate(startTime, envChan)
         }
    }
}

// updateClimate applica variazioni alla temperatura e umidità
func (e *Environment) updateClimate(deltaTemp, deltaHum float64, envChan chan<- models.EnvironmentState) {
	e.mutex.Lock()

	// Modifichiamo lo stato
	e.state.Temperature += deltaTemp
	e.state.Humidity += deltaHum

	// per ora non si presentano questi casi limite
	if e.state.Humidity < 0 {
		e.state.Humidity = 0
	} else if e.state.Humidity > 100 {
		e.state.Humidity = 100
	}

    e.state.Season = int(e.currentSeason)
	e.state.Timestamp = time.Now()

	stateCopy := e.state

	e.mutex.Unlock()
	// Inviamo lo stato modificato sul canale
	envChan <- stateCopy  //operazione bloccante in caso di saturazione del buffer
}

// Variazioni periodiche di temperatura e umidità per non avere tutti i giorni lo stesso comportamento
func (e *Environment) climateVariations(envChan chan<- models.EnvironmentState) {
	ticker := time.NewTicker(e.simCfg.ClimateVariationFreq)
	defer ticker.Stop() // Pulizia della risorsa quando la funzione esce
    defer e.wg.Done()

    for{
        select{
            case <- e.stopChan:
                log.Println("[INFO] climateVariations interrotto.")
                return

            case <-ticker.C:
                // Generazione variazione casuale di temperatura e umidità [-2, +2) e [-5, +5)
                deltaTemp := rand.Float64()*4 - 2
                deltaHum := rand.Float64()*10 - 5
                e.updateClimate(deltaTemp, deltaHum, envChan)
        }
    }
}

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

               // Applica il nuovo profilo climatico predefinito
               e.currentProfile = seasonProfiles[e.currentSeason]

                log.Printf("[STAGIONE] Nuova stagione avviata: %s!",
                    e.currentSeason.String())
                e.mutex.Unlock()
        }
    }
}