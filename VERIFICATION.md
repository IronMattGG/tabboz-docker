# Verifica locale — 29 ottobre 2026

## Correzione dei dialoghi

È stata riprodotta con un test la mancata liberazione dei controlli nella
revisione originale di Novantotto: `ReleaseHandle` confrontava un puntatore
`RESOURCE *` con l'handle numerico della finestra. I figli rimanevano allocati,
riducendo progressivamente i 100 slot disponibili. La correzione vendorizzata
confronta gli handle, rilascia ricorsivamente i figli e azzera gli slot al
rilascio. `DialogBox` conserva il risultato prima di distruggere lo slot.

Verifiche della correzione:

- Test originale fallito con un'asserzione sui figli rimasti allocati.
- 500 cicli con 24 controlli, un dialogo annidato e il suo controllo per ciclo:
  nessun handle residuo; finestre estranee preservate; slot di controlli riusati
  anche come finestre senza vecchi nomi di classe o callback.
- Stesso test nativo con AddressSanitizer e UndefinedBehaviorSanitizer superato.
  LeakSanitizer non utilizzabile nell'ambiente sotto ptrace; esclusa solo quella
  verifica, senza dichiarare una verifica generale di tutte le perdite di memoria.
- Test del risultato di `DialogBox`: 50 chiusure tramite callback, risultato
  corretto anche dopo il rilascio della finestra.
- Test compilati in WebAssembly ed eseguiti con Node nello stadio Emscripten
  della build Docker, quindi ricompilazione del gioco e ricreazione del container.
- Sulla build corretta: 301 file HTTP 200, MIME `application/wasm`, JavaScript
  sintatticamente valido, Wasm valido con le sette API del gioco richieste dal
  Makefile. Gli export interni del runtime possono differire rispetto al vecchio
  SDK: JavaScript e Wasm vengono generati insieme e serviti come coppia.
- Configurazioni Compose e Nginx valide, container healthy, porta ancora
  limitata a `127.0.0.1:8080`; sorgenti, SDK e test assenti dal server finale.

Questi test verificano il ciclo di vita dei dialoghi e delle risorse C, senza
simulare clic nel browser. Il servizio browser continua a restituire
`The browser access host is unavailable`: il problema riscontrato è compatibile
con i blocchi segnalati, ma la sequenza interattiva dell'utente non è stata
riprodotta e la correzione non dimostra l'assenza di ulteriori cause di blocco.

## Verifica iniziale della configurazione Docker (prima della correzione)

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
