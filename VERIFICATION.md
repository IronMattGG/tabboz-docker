# Verifica locale — 29 ottobre 2026

## Eseguito

- `docker compose config --quiet`: configurazione valida.
- `docker compose up --build -d`: immagine costruita e container avviato.
- `docker compose exec -T web nginx -t`: configurazione valida, Nginx 1.30.6.
- `docker compose ps`: servizio `web` healthy.
- Ispezione Docker: unica porta pubblicata `127.0.0.1:8080 → 80/tcp`.
- HTTP tramite `http://localhost:8080`: tutti i **301 file** dell'immagine
  finale rispondono 200; confronto dei byte con i file del progetto riuscito
  per gli asset del gioco. `zarrosim.wasm` restituisce `application/wasm`.
- Sorgenti C, Makefile, `.git/config`, `.env`, `AGENTS.md`, vecchi dialoghi
  di sviluppo e una risorsa inesistente rispondono 404.
- Tutte le immagini dichiarate in `resources/bitmaps/list.json` e i riferimenti
  locali in index, CSS e frammenti HTML dei dialoghi risultano presenti.
- `node --check` sui due JavaScript e `WebAssembly.compile` sul binario:
  sintassi e modulo validi (25 importazioni, 43 esportazioni). Questi controlli
  non eseguono il gioco nel browser.
- `docker compose restart web` e `docker compose up -d --force-recreate`:
  container riavviato/ricreato; HTTP verificato nuovamente sulla build finale.
- Esaminati fetch di Novantotto, font CSS, immagini e audio: gli URL di
  caricamento sono locali. L'unica immagine remota dell'index è stata copiata
  nel progetto. Restano collegamenti esterni e URL nelle attribuzioni, senza
  caricamenti automatici necessari da Internet.
- Contesto Docker limitato tramite allowlist; esclusi repository Git,
  directory personali/credenziali, sorgenti e materiali di sviluppo inutilizzati
  (compresi BMP Windows, ICO inutilizzati e frammenti JSON delle stringhe).
- `git diff --check`: nessun errore; nessun commit o push.

## Salvataggi: evidenza nel codice

`readkey.c` usa `RegCreateKeyEx`/`RegSetValue` e `RegOpenKeyEx`/`RegQueryValue`.
Le implementazioni incorporate in `zarrosim.js` leggono e scrivono JSON nel
`localStorage`; è stato esaminato anche `registry.c` di Novantotto per
confermare la composizione dei percorsi. Nessun filesystem del container
interviene nella persistenza.

`SalvaTutto()` viene chiamata nei punti descritti nel README, mentre
`FineProgramma()` salva all'uscita. Non è stata aggiunta una funzione di
salvataggio durante la chiusura della scheda. L'archivio browser sopravvive
normalmente a chiusura, riapertura e ricreazione del container; questa proprietà
non è stata verificata con una sessione interattiva in questo ambiente.

## Limiti riscontrati e prove ancora necessarie

Il servizio di controllo browser ha restituito due volte
`The browser access host is unavailable`, senza browser o app utilizzabili.
Non sono stati verificati avvio interattivo del gioco, finestre, azioni,
riproduzione audio dopo un clic, salvataggio e recupero dopo riapertura della
scheda o ricreazione del container, né una sessione di gioco senza Internet.
I controlli HTTP e la validazione Wasm non sono prove del gameplay.

Tre WAV richiamati dal codice sono assenti: `tabs0401.wav`, `tabs0601.wav` e
`tabs1400.wav`. Il confronto con la [cartella WAV del progetto web originale](https://github.com/andreax79/tabboz/tree/main/resources/wavs)
conferma che non sono inclusi neppure lì. Non sono stati inventati o sostituiti
effetti: questi richiami possono generare 404 e nessun suono. I 28 WAV presenti
sono tutti serviti correttamente.

Per completare la verifica interattiva con un browser disponibile:

1. Aprire `http://localhost:8080`, selezionare l'icona del gioco e completare
   eventuali finestre iniziali.
2. Aprire informazioni personali, configurazione, scuola e un negozio;
   eseguire un'azione rappresentativa e controllare immagini e testi.
3. Abilitare i suoni, interagire con un evento con WAV incluso e verificare
   la riproduzione; controllare Network/Console per eventuali altri errori.
4. Modificare un dato personale riconoscibile, confermare e uscire dal gioco
   con File → Chiudi. Chiudere e riaprire la scheda nello stesso profilo e
   verificare il recupero del dato.
5. Ripetere il controllo dopo `docker compose restart web`, poi dopo
   `docker compose up -d --force-recreate`, mantenendo lo stesso indirizzo.
6. Disconnettere Internet lasciando Docker avviato, ricaricare la pagina e
   ripetere alcune azioni. In Network verificare l'assenza di richieste esterne
   necessarie al gioco, distinguendo i tre WAV già mancanti nell'originale.
