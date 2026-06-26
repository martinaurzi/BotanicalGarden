package models

import "time"

// Stato ambiente in un dato istante
type EnvironmentState struct {
	Temperature float64   `json:"temperature"` //tag delle struct per dire al programma come trasformare i dati in json
	Humidity    float64   `json:"humidity"`
	Light       float64   `json:"light"`
	Timestamp   time.Time `json:"timestamp"`
}

// Stato della pianta
type PlantState struct {
	ID      int32  `json:"id"`
	Species string `json:"species"`
	Health  int32  `json:"health"` // 0-100
	Growth  int32  `json:"growth"` // 0-10
	Stage   string `json:"stage"`  // germoglio, piantina, piantaadulta, morta
}
