package storage

import (
    "time"
    "database/sql"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/environment"
    _ "github.com/mattn/go-sqlite3"
)

// Inizializza il database SQLite e crea la tabella degli snapshot se non esiste
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
            type TEXT,
            health INTEGER,
            growth INTEGER,
            stage TEXT,
            is_dead BOOLEAN,
            timestamp TEXT,
            season TEXT,
            temperature REAL,
            humidity REAL,
            light REAL
        );
    `)

    return db, err
}

// Salva lo stato corrente di tutte le piante e dell'ambiente mediante una transazione atomica
func SaveSnapshot(db *sql.DB, plants []models.PlantState, env models.EnvironmentState) error {
    tx, err := db.Begin()
    if err != nil {
       return err
    }
    defer tx.Rollback()

    stmt, err := tx.Prepare(`
        INSERT INTO plant_snapshots
        (plant_id, species, type, health, growth, stage, is_dead, timestamp, season, temperature, humidity, light)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`)
    if err != nil {
        return err
    }
    defer stmt.Close()

    // Formattazione del timestamp nel formato (dataTora+fuso)
    formattedTime := env.Timestamp.Format(time.RFC3339)
    // Conversione dell'intero rappresentante la stagione nella stringa corrispondente
    seasonString := environment.Season(env.Season).String()

    for _, p := range plants {
       _, err := stmt.Exec(
          p.ID, p.Name, p.Type, p.Health, p.Growth, p.GrowthStage, p.IsDead, formattedTime, seasonString,
          env.Temperature, env.Humidity, env.Light,
       )
       if err != nil {
          return err
       }
    }

    return tx.Commit()
}

// Svuota la tabella plant_snapshots
func ClearSnapshots(db *sql.DB) error {
    _, err := db.Exec(`DELETE FROM plant_snapshots`)
    if err != nil {
        return err
    }

    // Per azzerare il contenggio dell'autoincrement dell'id
    _, err = db.Exec(`DELETE FROM sqlite_sequence WHERE name='plant_snapshots'`)
    return err
}
