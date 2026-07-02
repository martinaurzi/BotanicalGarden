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

def plot_health_and_environment(df):
    """Genera un grafico dell'andamento della salute delle piante rispetto alla temperatura e umidità."""
    plt.figure(figsize=(14, 7)) #crea una nuova figura, figsize accetta una tupla, dove si indicano le dimensione in pollici, o se si mette un terzo elemento si indica l'unità di misura
    sns.set_theme(style="whitegrid") #set sfondo

    # Creiamo un grafico a linee per vedere la salute media delle specie nel tempo
    # L'argomento 'hue' separa automaticamente le linee per ogni specie di pianta
    ax1 = sns.lineplot(data=df, x='timestamp', y='health', hue='species', marker="o", linewidth=2.5, errorbar=None) #marker disegna un pallino in corrispondenza di ogni dato reale, hue crea linee diverse sulla base della colonna 'species', errorbar=None per togliere l'intervallo di confidenza di default del 95%
    plt.title("Andamento della Salute delle Piante e Variabili Ambientali", fontsize=16, fontweight='bold')
    plt.xlabel("Tempo", fontsize=12)
    plt.ylabel("Salute (0-100)", fontsize=12)
    plt.ylim(-5, 105) #impostiamo i limiti massimi e minimi dell'asse y sulla salute(visualizzazione più pulita)

    # Creiamo un secondo asse y che condivide lo stesso asse x, per sovrapporre la temperatura dell'ambiente
    ax2 = ax1.twinx() #ax1, rappresenta il grafico a assi cartesiani
    sns.lineplot(data=df, x='timestamp', y='temperature', color='red', alpha=0.4, linestyle='--', ax=ax2, label='Temp Ambiente (°C)', errorbar=None) #alpha per settare la trasparenza al 40% e non coprire la linea della salute delle piante, ax serve per indicare su quale disegnare la linea
    ax2.set_ylabel("Temperatura (°C)", color='red', fontsize=12)
    ax2.tick_params(axis='y', labelcolor='red') #per colorare i numeri dei gradi
    ax2.grid(False) # Evita la sovrapposizione delle linee di griglia

    #plt.tight_layout() non serve secondo me
    plt.savefig("salute_vs_ambiente.png", dpi=300) #salvataggio grafico come immagine
    plt.show() #mostrare il grafico pop up

def plot_growth_by_season(df):
    """Genera un grafico a barre (o boxplot) che mostra lo stato di crescita medio nelle varie stagioni."""
    plt.figure(figsize=(10, 6))
    sns.set_theme(style="ticks")

    sns.barplot(data=df, x='season', y='growth', hue='species', palette="YlGnBu")

    plt.title("Livello di Crescita delle Piante in Base alle Stagioni", fontsize=14, fontweight='bold')
    plt.xlabel("Stagione", fontsize=12)
    plt.ylabel("Livello di Crescita (0-10)", fontsize=12)
    plt.ylim(0, 11)
    plt.legend(title="Specie Pianta")

    plt.tight_layout()
    plt.savefig("crescita_stagioni.png", dpi=300)
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
            plot_health_and_environment(data_frame)
            plot_growth_by_season(data_frame)
            print("Grafici salvati come immagini nella cartella corrente.")

    except Exception as e:
        print(f"Errore durante l'esecuzione dello script: {e}")