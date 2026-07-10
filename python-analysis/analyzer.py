import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import seaborn as sns

def load_data_from_db(db_path="plants.db"):
    """Si connette al database SQLite, estrae la cronologia degli snapshot e la carica in un DataFrame Pandas"""

    query = "SELECT * FROM plant_snapshots ORDER BY id ASC"

    try:
        with sqlite3.connect(db_path) as conn:
            df = pd.read_sql_query(query, conn)

        if not df.empty:
            # Convertiamo la colonna dei timestamp in datetime per la gestione degli assi temporali nei grafici
            df['timestamp'] = pd.to_datetime(df['timestamp'])
        return df

    except pd.errors.DatabaseError as pd_err:
        print(f" [Errore Pandas SQL] Struttura DB non valida o tabella mancante.")
        raise RuntimeError(f"Errore nella query o tabella inesistente: {pd_err}")
    except sqlite3.Error as sql_err:
        print(f" [Errore Database] Impossibile connettersi o comunicare con SQLite: {sql_err}")
        raise RuntimeError(f"Errore durante la lettura del database: {sql_err}")

def plot_health_and_environment(df, env_column, env_label, color, ):
    """Genera un grafico a doppio asse Y per confrontare la salute delle piante con una variabile ambientale."""
    # Ordiniamo il dataframe cronologicamente e sulla base della specie
    df = df.sort_values(by=['species', 'timestamp'])

    # Includiamo i record in cui le piante sono in vita e il primo istante in cui avviene il decesso
    alive_mask = (df['is_dead'] == 0) | (df['is_dead'].diff() == 1)
    df_filtered = df[alive_mask]

    plt.figure(figsize=(14, 7))
    sns.set_theme(style="whitegrid")

    # Asse Y principale: andamento della salute delle varie specie nel tempo
    ax1 = sns.lineplot(data=df_filtered, x='timestamp', y='health', hue='species', marker="o", linewidth=2.5, errorbar=None)

    plt.title(f"Andamento della Salute delle Piante vs {env_label.split('(')[0].strip()}", fontsize=16, fontweight='bold')
    plt.xlabel("Tempo (MM:SS)", fontsize=12)
    plt.ylabel("Salute (0-100)", fontsize=12)
    plt.legend(title="Specie")

    # Generazione dei tick temporali ogni 3 secondi sulla base della durata reale della simulazione
    start = df_filtered['timestamp'].min()
    end = df_filtered['timestamp'].max()
    ticks = pd.date_range(start=start, end=end, freq='3s')

    ax1.set_xticks(ticks)
    ax1.xaxis.set_major_formatter(mdates.DateFormatter('%M:%S'))

    # Asse Y secondario: sovrapposizione della curva della variabile ambientale scelta
    ax2 = ax1.twinx()
    sns.lineplot(data=df, x='timestamp', y=env_column, color=color, alpha=0.4, linestyle='--', ax=ax2, label=env_label, errorbar=None)
    ax2.set_ylabel(env_label, color=color, fontsize=12)
    ax2.tick_params(axis='y', labelcolor=color)
    ax2.grid(False)

    filename = f"Salute_vs_{env_label.split('(')[0].strip()}.png"
    plt.savefig(f"plots/{filename}", dpi=300)
    plt.show()

def plot_plant_count_by_type_and_species(df):
    """Mostra il numero di piante per tipo, suddivise per specie."""

    # Consideriamo ogni pianta una sola volta, distinguendole mediante il plant_id
    df = df.drop_duplicates(subset="plant_id")

    # Creiamo la tabella di contingenza Tipo x Specie
    num_species_x_type = df.groupby(["type", "species"]).size().unstack(fill_value=0)
    #print(num_species_x_type)

    sns.set_theme(style="whitegrid")

    # Disegniamo il grafico a barre
    num_species_x_type.plot(kind="bar", stacked=True, figsize=(10, 6), colormap="tab20")

    plt.title("Distribuzione delle piante per tipo e specie")
    plt.xlabel("Tipo")
    plt.ylabel("Numero di piante")
    plt.xticks(rotation=0) # Allinea le etichette del tipo in orizzontale

    plt.legend(title="Specie")
    plt.tight_layout()
    plt.savefig("plots/conteggio_totale_piante_tipo.png", dpi=300)
    plt.show()

def plot_mortality_conditions(df):
    """Mostra in quali condizioni di temperatura e umidità avviene il decesso delle piante."""

    # Isoliamo il primo record in cui ogni piante muore
    df_dead = df[df['is_dead'] == 1].drop_duplicates(subset=['plant_id'])

    if df_dead.empty:
        print("Nessuna pianta è morta nella simulazione. Grafico mortalità saltato.")
        return

    plt.figure(figsize=(10, 6))
    sns.set_theme(style="ticks")

    # Scatter plot(grafico di dispersione): ogni punto è una pianta che muore, posizionata in base a Temperatura e Umidità del momento
    sns.scatterplot(data=df_dead, x='temperature', y='humidity', hue='type', style='species', s=100, palette="Set1", alpha=0.7)

    plt.title("Condizioni Ambientali Critiche al Momento del Decesso", fontsize=14, fontweight='bold')
    plt.xlabel("Temperatura di Shock (°C)", fontsize=12)
    plt.ylabel("Umidità di Shock (%)", fontsize=12)

    plt.savefig("plots/condizioni_mortalita.png", dpi=300)
    plt.show()

def plot_stage_distribution(df):
    """Mostra la distribuzione percentuale degli stadi di sviluppo delle piante per ogni stagione."""
    plt.figure(figsize=(10, 6))

    # Creiamo una tabella di contingenza: calcola le frequenze relative degli stadi di crescita normalizzate per singola stagione
    stage_counts = pd.crosstab(df['stage'], df['season'], normalize='columns') * 100
    #print(stage_counts)

    # Disegniamo la Heatmap
    sns.heatmap(stage_counts, annot=True, fmt=".1f", cmap="YlGn", cbar_kws={'label': '% sul totale delle piante'})

    plt.title("Percentuale degli Stadi di Sviluppo nelle Varie Stagioni", fontsize=14, fontweight='bold')
    plt.xlabel("Stagione", fontsize=12)
    plt.ylabel("Stadio di Crescita", fontsize=12)

    plt.savefig("plots/heatmap_stadi_crescita.png", dpi=300)
    plt.show()

def plot_final_survival_by_season(df):
    """Mostra quante piante sopravvivono alla fine di ogni stagione."""

    # Consideriamo solo le piante vive
    df_alive = df[df['is_dead'] == 0]

    # Determiniamo l'ultimo istante osservato per ogni stagione
    last_time = df_alive.groupby('season')['timestamp'].max()

    final_survivors = []

    # Contiamo le piante (distinte) rimaste in vita in corrispondenza dell'ultimo timestamp stagionale
    for season, time in last_time.items():
        plants = df_alive[ (df_alive['season'] == season) &(df_alive['timestamp'] == time)]['plant_id'].nunique()

        final_survivors.append({
            'season': season,
            'alive_plants': plants
        })

    survival_df = pd.DataFrame(final_survivors)

    plt.figure(figsize=(8,5))

    sns.barplot(data=survival_df,x='season',y='alive_plants')

    plt.title("Numero di piante sopravvissute per stagione", fontsize=14, fontweight='bold')
    plt.xlabel("Stagione")
    plt.ylabel("Piante vive alla fine della stagione")

    plt.savefig("plots/piante_sopravvissute_fine_stagione.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    print("Caricamento dati dal database")
    try:
        data_frame = load_data_from_db("../go-simulator/plants.db")

        if data_frame.empty:
            print("Il database è vuoto. Avvia prima la simulazione in Go/C++!")
        else:
            print(f"Dati caricati con successo. Trovati {len(data_frame)} snapshot.")

            # Generazione dei grafici
            print("Generazione dei grafici in corso")
            plot_plant_count_by_type_and_species(data_frame)
            plot_health_and_environment(data_frame, 'temperature', 'Temperatura (°C)', 'red')
            plot_health_and_environment(data_frame, 'humidity', 'Umidità (%)', 'blue')
            plot_health_and_environment(data_frame, 'light', 'Luce (%)', 'orange')
            plot_stage_distribution(data_frame)
            plot_final_survival_by_season(data_frame)
            plot_mortality_conditions(data_frame)
            print("Grafici salvati come immagini nella cartella 'plots'.")

    except RuntimeError as e: # rendere il main indipendente dal tipo di database che uso
        print(f"Errore Runtime: {e}")
    except Exception as e:
        print(f"Errore: {e}")