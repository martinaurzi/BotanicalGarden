package storage

import (
    "time"
    "database/sql" //interfaccia generica per i database
    "github.com/martinaurzi/BotanicalGarden/go-simulator/pkg/models"
    _ "github.com/mattn/go-sqlite3" //driver specifico per sqlite
    //_ per il blank import in quanto noi non facciamo chiamate specifiche a go-sqlite3 ma alle funzione di sql, senza blank import il compilatore si blocca perchè tutto cio che viene importato deve essere usato
)

// preparazione del database al percorso path
func InitDB(path string) (*sql.DB, error) {
    db, err := sql.Open("sqlite3", path) //mi restituisce in db il gestore della connessione con il database
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
    `) //abbiamo ignota con _ sql.Result che ha due metodi che ci restituisce il numero di righe modficate e l'ultimo id generato con autoincrement

    return db, err
}

func SaveSnapshot(db *sql.DB, plants []models.PlantState, env models.EnvironmentState) error {
    tx, err := db.Begin()
    if err != nil {
       return err
    }
    defer tx.Rollback()

    // Prepariamo la query una volta sola nel database
    stmt, err := tx.Prepare(`
        INSERT INTO plant_snapshots
        (plant_id, species, type, health, growth, stage, is_dead, timestamp, season, temperature, humidity, light)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`)
    if err != nil {
        return err
    }
    defer stmt.Close() // Chiude lo statement alla fine della funzione

    // Formattiamo il timestamp una volta sola fuori dal ciclo
    formattedTime := env.Timestamp.Format(time.RFC3339)

    for _, p := range plants {
       // Usiamo stmt.Exec che è molto più veloce di tx.Exec dentro un ciclo
       _, err := stmt.Exec(
          p.ID, p.Name, p.Type, p.Health, p.Growth, p.GrowthStage, p.IsDead, formattedTime, env.Season,
          env.Temperature, env.Humidity, env.Light,
       )
       if err != nil {
          return err
       }
    }

    return tx.Commit()
}

func ClearSnapshots(db *sql.DB) error {
    _, err := db.Exec(`DELETE FROM plant_snapshots`)
    if err != nil {
        return err
    }

    // azzera l'autoincrement dell'id
    _, err = db.Exec(`DELETE FROM sqlite_sequence WHERE name='plant_snapshots'`)
    return err
}
