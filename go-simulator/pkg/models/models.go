package models

import "time"

// Stato ambiente in un dato istante
type EnvironmentState struct {
    Temperature float64 `json:"temperature"` //tag delle struct per dire al programma come trasformare i dati in json
    Humidity float64 `json:"humidity"`
    Light float64 `json:"light"`
    Season string `json:"season"`
    Timestamp time.Time `json:"timestamp"`
}

// Stato della pianta
type PlantState struct {
    ID          int     `json:"id"`
    Type        string  `json:"type"`
    Name        string  `json:"name"` //indica la specie
    Health      float64 `json:"health"`
    Growth      float64 `json:"growth"`
    GrowthStage string  `json:"growth_stage"`
    IsDead      bool    `json:"is_dead"`
}