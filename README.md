# Tabboz Simulator — versione Docker

Tabboz Simulator con la grafica Windows 95/98 della conversione web originale,
avviabile con Docker Compose. Il gioco gira nel browser tramite
JavaScript e WebAssembly; Nginx serve i file dal container.

## Novità di questa versione

- Avvio con un solo comando, senza installare Wine, Node, Python o Emscripten.
- Accesso locale su **http://localhost:8080**, con porta pubblicata solo su `127.0.0.1`.
- Risorse distribuite con il gioco disponibili localmente, compresi font,
  immagini e suoni, per l'uso senza Internet dopo la build.
- Correzione della gestione dei dialoghi di Novantotto: la chiusura libera i
  controlli figli e permette di riutilizzare le risorse dopo aperture ripetute.
- Compilazione multistadio con versioni fissate e test di regressione eseguiti
  durante la build. Il container finale contiene soltanto il server e i file web.

## Requisiti

Docker Engine avviato e plugin **Docker Compose v2**. Per la prima build serve
Internet per scaricare le immagini Emscripten e Nginx. Gli strumenti di
compilazione sono inclusi nello stadio di build Docker.

## Installazione e avvio

```sh
git clone https://github.com/IronMattGG/tabboz-docker.git
cd tabboz-docker
docker compose up --build -d
```

Se hai già scaricato il progetto, esegui soltanto l'ultimo comando dalla sua
directory. Apri **http://localhost:8080** e fai clic sull'icona
**Tabboz Simulator**.

Il container deve rimanere avviato mentre giochi. Dopo la build non occorrono
servizi esterni per caricare le risorse incluse; il collegamento GitHub apre
un sito esterno soltanto se selezionato. Non aprire `index.html` tramite
`file://`: il gioco carica le risorse via HTTP.

## Gestione e aggiornamenti

```sh
# Stato e log
docker compose ps
docker compose logs --tail=50 web

# Arresto e rimozione del container
docker compose down

# Avvio dell'immagine già costruita
docker compose up -d

# Riavvio del servizio
docker compose restart web
```

Per aggiornare una copia clonata con Git, dalla directory del progetto:

```sh
git pull --ff-only
docker compose up --build -d
```

Poi ricarica il browser con **Ctrl+Shift+R**, senza cancellare i dati del sito.
Se hai modifiche locali, conservale prima di aggiornare i sorgenti.
Le immagini base sono fissate tramite digest nel [Dockerfile](Dockerfile):
per aggiornare anche SDK o Nginx occorre modificare quei riferimenti e ricostruire.

## Salvataggi

La partita viene salvata nel **localStorage del browser**, nel suo profilo,
per l'indirizzo `http://localhost:8080`. I dati non risiedono nel container:
un volume Docker non ne costituisce un backup. Arrestare, ricostruire o
ricreare il container non cancella l'archivio del browser.

Il gioco salva a fine giornata, alla chiusura delle finestre delle informazioni
personali o della configurazione e all'uscita tramite **File → Chiudi...**.
Prima di chiudere la scheda, usa l'uscita del gioco e confermala. La chiusura
improvvisa della scheda può perdere le azioni successive all'ultimo salvataggio.
Le voci **Apri...** e **Salva con nome...** della conversione web sono disabilitate.

Per ritrovare la partita, usa sempre lo stesso browser, profilo, indirizzo e
porta. `localhost` e `127.0.0.1` hanno archivi separati; anche cambiare porta
crea un'origine diversa. Evita navigazione privata, cancellazione dei dati
del sito e pulitori automatici. Prima di eliminare il profilo o cambiare
computer, esegui una copia del profilo con il browser chiuso, secondo le
istruzioni del browser utilizzato.

Per ispezionare i dati: strumenti sviluppatore → **Application/Archiviazione →
Local Storage → http://localhost:8080**. Le chiavi iniziano con
`HKEY_CURRENT_USER\Software\Obscured Truckware\Tabboz Simulator 32`.
La compatibilità con i vecchi salvataggi Windows non è stata verificata.

## Correzione dei dialoghi e compilazione

La versione originale di Novantotto non liberava i controlli figli alla
chiusura delle finestre. Aperture ripetute potevano esaurire la tabella di
100 handle e bloccare i dialoghi. Questa versione rilascia i figli
ricorsivamente, azzera gli slot prima del riuso e conserva il risultato del
dialogo prima di distruggerlo. La logica del gioco e il formato dei salvataggi
rimangono quelli originali.

La build usa **Emscripten 3.1.56**, **Nginx 1.30.6** e Novantotto alla revisione
`12176b4d630bfb2d7426ff8b5ddf81bbc875e550`, incluso in `vendor/novantotto` con
le correzioni locali. Anche la data incorporata dal compilatore è fissata.
I dettagli sono in [UPSTREAM.md](vendor/novantotto/UPSTREAM.md).

Docker esegue `make test build` e copia nel server JavaScript e WebAssembly
appena generati. I file `zarrosim.js` e `zarrosim.wasm` storici alla radice del
repository non vengono usati dall'immagine Docker.
Per eseguire soltanto lo stadio di compilazione e test:

```sh
docker build --target build -t tabboz-build .
```

## Verifiche e limiti noti

Sono stati verificati build e avvio reali, riavvio e ricreazione del container,
stato `healthy`, configurazioni Compose/Nginx, 301 file via HTTP e MIME
`application/wasm`. I test di regressione coprono 500 cicli di dialoghi
annidati e 50 verifiche del risultato restituito alla chiusura.

Le prove automatiche non sostituiscono la verifica interattiva del gameplay.
Il browser controllabile dell'ambiente di sviluppo era indisponibile:
audio, recupero dei salvataggi e una sessione senza Internet restano da
verificare con un browser. Risultati e procedura manuale sono in
[VERIFICATION.md](VERIFICATION.md).

Sono inclusi 28 WAV. Tre effetti richiamati dal codice mancano già nella
conversione web originale: `tabs0401.wav` (bocciatura), `tabs0601.wav` (rifiuto
della tipa) e `tabs1400.wav` (metallone). Quegli eventi possono restare senza
suono e generare richieste HTTP 404.

## Problemi comuni

- **Docker non raggiungibile o permission denied:** verifica che il daemon sia
  avviato e che il tuo utente abbia accesso a Docker.
- **Porta 8080 occupata:** arresta il servizio che la usa oppure modifica la
  porta host in [compose.yaml](compose.yaml), mantenendo `127.0.0.1`. Alla nuova
  porta i salvataggi saranno separati; quelli precedenti restano all'origine originale.
- **Pagina vecchia o dialoghi ancora bloccati dopo l'aggiornamento:** ricostruisci
  con `docker compose up --build -d` e ricarica con **Ctrl+Shift+R**. Se il blocco
  si ripete, annota il dialogo, i passaggi e gli errori della console del browser.
- **Nessun suono:** interagisci con il gioco, abilita i suoni nella configurazione
  e controlla che scheda e uscita audio non siano silenziate. Alcuni browser
  bloccano l'audio prima di un'interazione; tre effetti sono assenti come indicato sopra.
- **Partita mancante:** controlla browser, profilo, indirizzo, porta e impostazioni
  di cancellazione automatica. Non cancellare i dati del sito per svuotare la cache.

## Progetto originale e licenze

Questa versione mantiene grafica e contenuti della
[conversione web di Andrea Bonomi](https://github.com/andreax79/tabboz), basata su
[Novantotto](https://github.com/andreax79/novantotto),
[Emscripten](https://emscripten.org/) e [98.css](https://github.com/jdan/98.css).

Tabboz Simulator è distribuito secondo la licenza **GPL-3.0-or-later** riportata
nei sorgenti; il testo è in [LICENSE](LICENSE). Attribuzioni e licenze dei
componenti sono in [THIRD_PARTY.md](THIRD_PARTY.md) e nella directory `licenses/`,
incluse anche nell'immagine Docker.
