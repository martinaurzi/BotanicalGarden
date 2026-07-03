import sqlite3 #modulo nativo di python
import pandas as pd #libreria per manipolazione dati e lavorare in modo efficiente con dati strutturati come tabelle SQL
import matplotlib.pyplot as plt #per la struttura di base del grafico e gestione degli assi
import seaborn as sns #libreria per visualizzazione dei dati costruita sopra matplotlib, per costruzione di grafiche statiche

def load_data_from_db(db_path="plants.db"):
    """Si connette al database SQLite e carica i dati in un DataFrame Pandas."""
    conn = sqlite3.connect(db_path)

    # Query per estrarre tutti i dati dei snapshot ordinati per tempo
    query = "SELECT * FROM plant_snapshots ORDER BY id ASC"

    # Pandas converte automaticamente la tabella SQL in un DataFrame
    df = pd.read_sql_query(query, conn) #ritorna un DataFrame(tabella con righe e colonne) corrispondente al risultato della query
    conn.close()

    # Convertiamo la colonna timestamp in un formato datetime per i grafici temporali
    df['timestamp'] = pd.to_datetime(df['timestamp']) #si passa come argomento l'oggetto da convertire(in questo caso, scalare timestamp) in datetime
    return df

def plot_health_and_environment(df, env_column, env_label, color, ):
    """Genera un grafico dell'andamento della salute delle piante rispetto alla temperatura e umidità."""
    # Ordiniamo per sicurezza il dataframe cronologicamente
    df = df.sort_values(by=['species', 'timestamp'])

    # Teniamo i record vivi (is_dead == 0) e il primo record in cui muore
    # creiamo una maschera che include i record vivi e quelli dove is_dead è appena cambiato a 1
    alive_mask = (df['is_dead'] == 0) | (df['is_dead'].diff() == 1)
    df_filtrato = df[alive_mask]

    plt.figure(figsize=(14, 7)) #crea una nuova figura, figsize accetta una tupla, dove si indicano le dimensione in pollici, o se si mette un terzo elemento si indica l'unità di misura
    sns.set_theme(style="whitegrid") #set sfondo

    # Creiamo un grafico a linee per vedere la salute media delle specie nel tempo
    # L'argomento 'hue' separa automaticamente le linee per ogni specie di pianta
    ax1 = sns.lineplot(data=df_filtrato, x='timestamp', y='health', hue='species', marker="o", linewidth=2.5, errorbar=None) #marker disegna un pallino in corrispondenza di ogni dato reale, hue crea linee diverse sulla base della colonna 'species', errorbar=None per togliere l'intervallo di confidenza di default del 95%
    plt.title(f"Andamento della Salute delle Piante vs {env_label.split('(')[0].strip()}", fontsize=16, fontweight='bold')
    plt.xlabel("Tempo", fontsize=12)
    plt.ylabel("Salute (0-100)", fontsize=12)

    # Creiamo un secondo asse y che condivide lo stesso asse x, per sovrapporre la temperatura dell'ambiente
    ax2 = ax1.twinx() #ax1, rappresenta il grafico a assi cartesiani
    sns.lineplot(data=df, x='timestamp', y=env_column, color=color, alpha=0.4, linestyle='--', ax=ax2, label=env_label, errorbar=None) #alpha per settare la trasparenza al 40% e non coprire la linea della salute delle piante, ax serve per indicare su quale disegnare la linea
    ax2.set_ylabel(env_label, color=color, fontsize=12)
    ax2.tick_params(axis='y', labelcolor=color) #per colorare i numeri dei gradi
    ax2.grid(False) # Evita la sovrapposizione delle linee di griglia

    filename = f"salute_vs_{env_label.split('(')[0].strip()}.png"
    plt.savefig(filename, dpi=300) #salvataggio grafico come immagine
    plt.show() #mostrare il grafico pop up

def plot_growth_by_type_and_season(df):
    """Mostra come crescono i diversi macro-tipi di piante nelle varie stagioni."""
    plt.figure(figsize=(10, 6))
    sns.set_theme(style="whitegrid")

    # Raggruppiamo per 'type' (Desert, Tropical, Temperate) sull'asse x e separiamo per stagione con 'hue'
    sns.barplot(data=df, x='type', y='growth', hue='season', palette="YlOrBr", errorbar=None)

    plt.title("Impatto delle Stagioni sulla Crescita per Tipo di Vegetazione", fontsize=14, fontweight='bold')
    plt.xlabel("Tipo di Pianta", fontsize=12)
    plt.ylabel("Crescita Media", fontsize=12)
    plt.legend(title="Stagione")

    plt.tight_layout()
    plt.savefig("crescita_tipo_stagione.png", dpi=300)
    plt.show()

def plot_mortality_conditions(df):
    """Mostra in quali condizioni di temperatura e umidità avviene il decesso delle piante."""
    # Filtriamo solo i record in cui la pianta è effettivamente morta
    df_morte = df[df['is_dead'] == 1].drop_duplicates(subset=['plant_id'])

    if df_morte.empty:
        print("Nessuna pianta è morta nella simulazione. Grafico mortalità saltato.")
        return

    plt.figure(figsize=(10, 6))
    sns.set_theme(style="ticks")

    # Scatter plot: ogni punto è una pianta che muore, posizionata in base a Temp e Umidità del momento
    sns.scatterplot(data=df_morte, x='temperature', y='humidity', hue='type', style='species', s=100, palette="Set1")

    plt.title("Condizioni Ambientali Critiche al Momento del Decesso", fontsize=14, fontweight='bold')
    plt.xlabel("Temperatura di Shock (°C)", fontsize=12)
    plt.ylabel("Umidità di Shock (%)", fontsize=12)

    plt.tight_layout()
    plt.savefig("condizioni_mortalita.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    print("Caricamento dati dal database...")
    try:
        data_frame = load_data_from_db("../go-simulator/plants.db")

        if data_frame.empty:
            print("Il database è vuoto. Avvia prima la simulazione in Go/C++!")
        else:
            print(f"Dati caricati con successo. Trovati {len(data_frame)} snapshot.")

            # Generazione dei grafici
            print("Generazione dei grafici in corso...")
            plot_health_and_environment(data_frame, 'temperature', 'Temperatura (°C)', 'red')
            plot_health_and_environment(data_frame, 'humidity', 'Umidità (%)', 'blue')
            plot_health_and_environment(data_frame, 'light', 'Luce (%)', 'orange')
            plot_growth_by_type_and_season(data_frame)
            plot_mortality_conditions(data_frame)
            print("Grafici salvati come immagini nella cartella corrente.")

    except Exception as e:
        print(f"Errore durante l'esecuzione dello script: {e}")