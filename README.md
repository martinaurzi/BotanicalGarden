## Guida all'Installazione e all'Avvio

Seguire questi passaggi nell'ordine indicato per configurare l'ambiente virtuale Python, compilare la libreria in C++ ed eseguire la simulazione in Go con la successiva analisi dei dati.

---

### 1. Creazione e attivazione dell'ambiente virtuale (venv) e installazione requirements (Python)

Spostarsi nella cartella `python-analysis` ed eseguire i seguenti comandi nel terminale:

```bash
py -m venv .venv

Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process

.\.venv\Scripts\Activate.ps1 

pip install -r requirements.txt  
```


### 2. Build della libreria C++

Aprire un nuovo terminale e spostarsi nella cartella `cpp_plant_modeling` ed eseguire i seguenti comandi nel terminale:

```bash
cmake -S . -B cmake-build-debug -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

cmake --build cmake-build-debug
```

### 3. Avvio simulazione Go

Spostarsi nella cartella `go-simulator` ed eseguire il seguente comando nel terminale:

```bash
go run main.go
```

### 4. Visualizzazione grafici Python

Riprendere il terminale relativo al passo uno e accertarsi di essere nella cartella `python-analysis` ed eseguire il seguente comando nel terminale:

```bash
python analyzer.py  
```
