package simulation

import (
    "go-simulator/pkg/models"
    "go-simulator/pkg/plant"
)

func StartPlantRoutine(
    p *plant.Plant,
    envChan <-chan models.EnvironmentState,
    snapshotChan chan<- models.PlantState,
) {
    go func() {
        for env := range envChan {
            p.Update(env)
            snapshotChan <- p.State
        }
    }()
}
