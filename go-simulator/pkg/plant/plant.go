package plant

import (
    "encoding/json"
    "go-simulator/pkg/models"
)

type Plant struct {
    State models.PlantState
}

// Funzione temporanea non finisce il C++
func CallCppUpdate(jsonData string) string {
    // Restituiamo un JSON statico per testare se Go e il DB funzionano
    return `{"health": 85, "growth": 1, "stage": "germoglio"}`
}

// Funzione per aggiornare la pianta
func (p *Plant) Update(env models.EnvironmentState) error {

    req := models.PlantUpdateRequest{
        Plant: p.State,
        Env: env,
    }

    data, err := json.Marshal(req) //marshall restituisce converte la struct Goin json, restituendo un array di byte e un error
    if err != nil {
        return err
    }

    //CallCppUpdate va implementata usando la funzione C++
    responseJSON := CallCppUpdate(string(data))

    var resp models.PlantUpdateResponse
    // responseJSON è una string quindi va riportata in []byte
    if err := json.Unmarshal([]byte(responseJSON), &resp); err != nil {
        return err //con questa scrittura la variabile err esiste solo all'interno dell'if
    }

    // aggiornamento stato
    p.State.Health = resp.Health
    p.State.Growth = resp.Growth
    p.State.Stage = resp.Stage
    p.State.Timestamp = env.Timestamp

    return nil
}