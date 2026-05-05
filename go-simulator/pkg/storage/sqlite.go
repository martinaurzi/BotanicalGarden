package storage

import (
    "database/sql"
    "go-simulator/pkg/models"
    _ "github.com/mattn/go-sqlite3"
)

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