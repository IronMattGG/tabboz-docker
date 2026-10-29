# Tabboz Simulator su Linux con Docker

Questa versione serve la conversione web originale: grafica Windows 95/98,
JavaScript, WebAssembly, font e suoni già inclusi. Il gioco viene eseguito nel
browser; il container contiene soltanto Nginx e i file statici necessari.

## Avvio

Occorrono Docker Engine avviato e il plugin Docker Compose v2. Non servono
Node, Python, Emscripten o Wine sul computer dell'utente.

Dalla directory del progetto:

```sh
docker compose up --build -d
```

Aprire **http://localhost:8080** e fare clic sull'icona **Tabboz Simulator**.
La porta è pubblicata esclusivamente su `127.0.0.1`, per l'accesso locale.
Il primo download delle immagini Emscripten e Nginx richiede Internet; dopo la build tutti
i file distribuiti con il gioco sono locali, compresi suoni, font e nastro GitHub.
Il collegamento GitHub apre un sito esterno soltanto se selezionato.
Il container deve rimanere avviato anche quando Internet non è disponibile.
Non aprire `index.html` tramite `file://`: le risorse vengono caricate via HTTP.

```sh
# Stato e diagnostica
docker compose ps
docker compose logs --tail=50 web

# Arresto e rimozione del container
docker compose down

# Riavvio senza ricostruzione
docker compose up -d

# Dopo aver aggiornato i file del progetto
docker compose up --build -d
```

La base Nginx è fissata tramite digest nel Dockerfile. Per aggiornare anche
Nginx bisogna aggiornare quel riferimento a una nuova immagine ufficiale,
quindi ricostruire. Il normale aggiornamento dei file non scarica una base diversa.

## Salvataggi

La conversione web usa già il `localStorage` del browser: `readkey.c` richiama
le funzioni di registro Novantotto, tradotte nel JavaScript incluso in accessi
al `localStorage`. I dati si trovano sotto il prefisso
`HKEY_CURRENT_USER\Software\Obscured Truckware\Tabboz Simulator 32`, con
una voce per ciascun valore del registro e contenuto JSON.

Il gioco salva a fine giornata, quando si chiudono le finestre delle informazioni
personali o della configurazione e quando si esce tramite **File → Chiudi...**.
Prima di chiudere la scheda, usare l'uscita del gioco e confermarla. Non è
implementato un salvataggio aggiuntivo alla chiusura improvvisa della scheda:
le azioni successive all'ultimo punto di salvataggio possono andare perse.
Le voci originali **Apri...** e **Salva con nome...** sono disabilitate nella
conversione web; non costituiscono esportazione/importazione di file.

I salvataggi rimangono nel profilo del browser dopo chiusura e riapertura,
riavvio, ricostruzione o rimozione del container. **Un volume Docker non li
protegge.** Usare sempre lo stesso browser, profilo e indirizzo:
`http://localhost:8080` e `http://127.0.0.1:8080` hanno archivi diversi, così
come porte diverse. Evitare navigazione privata, cancellazione dei dati del
sito, pulitori automatici e blocco dell'archiviazione locale.

Per proteggerli prima di eliminare un profilo o cambiare computer, fare una
copia del profilo del browser con il browser chiuso, secondo la documentazione
del browser utilizzato. I dati sono ispezionabili negli strumenti sviluppatore,
in **Application/Archiviazione → Local Storage → http://localhost:8080**.
Non è stata verificata la compatibilità con i vecchi salvataggi Windows.

## Problemi comuni

- **Docker non raggiungibile / permission denied:** verificare che il daemon sia
  avviato e che l'utente abbia accesso a Docker secondo la configurazione Linux.
- **Porta 8080 occupata:** arrestare il servizio che la usa, oppure cambiare solo
  la porta host in `compose.yaml`, mantenendo `127.0.0.1`. Il nuovo indirizzo avrà
  salvataggi separati: per ritrovare quelli precedenti tornare alla porta originale.
- **Pagina vecchia o errore di caricamento:** controllare `docker compose ps` e i
  log, poi ricaricare la pagina senza cache. Non cancellare i dati del sito per
  risolvere un problema di cache: contengono il salvataggio.
- **Nessun suono:** interagire con il gioco, verificare l'opzione suoni nella
  configurazione e che la scheda e l'uscita audio non siano silenziate. I WAV
  sono locali; il browser può bloccare l'audio prima di un'interazione.
- **Partita mancante:** verificare browser, profilo, indirizzo e porta originali,
  oltre alle impostazioni che cancellano i dati alla chiusura.

## Verifiche e limiti

Le verifiche locali sono descritte in `VERIFICATION.md`, distinguendo controlli
HTTP e dei file dalle prove interattive. È stata corretta in Novantotto la
liberazione dei controlli dei dialoghi: la versione originale lasciava occupati
gli slot della tabella di 100 handle e poteva esaurirli dopo aperture ripetute.
La chiusura libera anche i controlli annidati e azzera gli slot prima del riuso;
il valore restituito dal dialogo viene conservato prima della distruzione.
La logica del gioco e il formato dei salvataggi rimangono quelli originali.

Sono presenti 28 WAV. Tre effetti richiamati dal codice sono assenti già nella
distribuzione web originale: `tabs0401.wav` (bocciatura), `tabs0601.wav` (rifiuto
della tipa) e `tabs1400.wav` (metallone). Quegli eventi possono richiedere un
file inesistente e rimanere senza suono. Non sono stati sostituiti con effetti
diversi. Gameplay, riproduzione audio e persistenza effettiva nel browser
restano da verificare interattivamente: il servizio browser dell'ambiente
di lavoro non era disponibile.

La build Docker è multistadio: compila il gioco con Emscripten 3.1.56 e la
copia locale di Novantotto, fissata alla revisione
`12176b4d630bfb2d7426ff8b5ddf81bbc875e550`. Le immagini SDK e Nginx sono fissate
tramite digest; anche la data incorporata dal compilatore è costante.
La build esegue i test di regressione in WebAssembly, poi copia soltanto gli
artefatti generati nel server statico. Non occorre installare strumenti extra
sul computer dell'utente. I `zarrosim.js`/`zarrosim.wasm` storici alla radice
non vengono usati dall'immagine Docker; la versione corretta viene ricompilata
dal Makefile. Modifiche locali alla dipendenza: `vendor/novantotto/UPSTREAM.md`.

Dopo un aggiornamento, eseguire `docker compose up --build -d` e ricaricare
il browser con **Ctrl+Shift+R**, senza cancellare i dati del sito.

Licenza originale in [LICENSE](LICENSE); attribuzioni e licenze dei componenti
in [THIRD_PARTY.md](THIRD_PARTY.md) e `licenses/`, incluse nell'immagine.

---

## 🖥️ Web development

<div align="center">
  <a href="https://andreax79.github.io/tabboz/" target="_blank"><img alt="Tabboz Simulator Screenshot" src="https://github.com/andreax79/tabboz/assets/1288154/0999ef77-3e7a-45a0-9a0b-1206a6f5bddd"/></a>
</div>
<br/>

### 📖 Prerequisites

In order to compile the project we need the following software installed on our development machine:
- Python >= 3.8
- [Emscripten](https://emscripten.org/)

### 📦 Builds

Build the project.

```shell
make build
```

We also have a script for updating dialogs used in the project, you only need to run this if you modify the `.rc` files:

```shell
make rc
```

### 🎨 Code linting

To fix the linting errors, use the following command:

```shell
make format
```

### 🚀 Links

- [Novantotto, A tool for porting Windows programs to WebAssembly](https://github.com/andreax79/novantotto)
- [Emscripten, A complete compiler toolchain to WebAssembly](https://emscripten.org/)
- [98.css, A design system for building faithful recreations of old UIs](https://github.com/jdan/98.css)

## ⚖️ LICENSE

[![GPL 3](https://www.gnu.org/graphics/gplv3-or-later.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)
