package storage

import (
	"database/sql"
	"go-simulator/pkg/models"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

// time per usare RFC3339

func InitDB(path string) (*sql.DB, error) {
	db, err := sql.Open("sqlite3", path)
	if err != nil {
		return nil, err
	}

	_, err = db.Exec(`
        CREATE TABLE IF NOT EXISTS plant_snapshots (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            plant_id INTEGER,
            species TEXT,
            health INTEGER,
            growth INTEGER,
            stage TEXT,
            timestamp TEXT,
            temperature REAL,
            humidity REAL,
            light REAL
        );
    `)

	return db, err
}

func SaveSnapshot(db *sql.DB, plants []models.PlantState, env models.EnvironmentState) error {
	tx, err := db.Begin() // Iniziamo la transazione atomica, per non riaprire il file ogni volta che inseriamo una pianta
	if err != nil {
		return err
	}
	defer tx.Rollback()

	for _, p := range plants {
		_, err := tx.Exec(`
			INSERT INTO plant_snapshots 
			(plant_id, species, health, growth, stage, timestamp, temperature, humidity, light) 
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			p.ID, p.Species, p.Health, p.Growth, p.Stage, env.Timestamp.Format(time.RFC3339),
			env.Temperature, env.Humidity, env.Light,
		)
		if err != nil {
			return err
		}
	}
	return tx.Commit() // Eseguiamo una transazione per fare tutti gli inserimenti nel Database.
}

/*
func InsertSnapshot(db *sql.DB, p models.PlantState, env models.EnvironmentState) error {
    _, err := db.Exec(`
        INSERT INTO plant_snapshots
        (plant_id, species, health, growth, stage, timestamp, temperature, humidity, light)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    `,
        p.ID, p.Species, p.Health, p.Growth, p.Stage, p.Timestamp,
        env.Temperature, env.Humidity, env.Light,
    )
    return err
}
*/
