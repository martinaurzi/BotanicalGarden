package models

import "time"

// Stato ambiente in un dato istante
type EnvironmentState struct {
    Temperature float64 `json:"temperature"` //tag delle struct per dire al programma come trasformare i dati in json
    Humidity float64 `json:"humidity"`
    Light float64 `json:"light"`
    Timestamp time.Time `json:"timestamp"`
}

// Stato della pianta
type PlantState struct {
    ID int `json:"id"`
    Species string `json:"species"`
    Health int `json:"health"` // 0-100
    Growth int `json:"growth"` // 0-10
    Stage string `json:"stage"` // germoglio, piantina, piantaadulta, morta
    Timestamp time.Time `json:"timestamp"`
}

// Struttura richiesta a C++
type PlantUpdateRequest struct {
    Plant PlantState `json:"plant"`
    Env EnvironmentState `json:"env"`
}

// Struttura risposta da C++
type PlantUpdateResponse struct {
    Health int `json:"health"`
    Growth int `json:"growth"`
    Stage string `json:"stage"`
}