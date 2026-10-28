# Obiettivo

Rendere questa versione di Tabboz Simulator utilizzabile su Linux attraverso
Docker, mantenendo grafica, contenuti e comportamento della conversione web
esistente. Portare il lavoro fino a un avvio locale documentato e verificato.
Non limitarsi a proporre un piano.

## Ambito e autorizzazione

- Implementare quanto necessario: configurazione Docker, Docker Compose,
  eventuali correzioni minime, documentazione e verifiche locali.
- Non riscrivere il gioco in Rust, GTK o altri framework. Non utilizzare Wine.
- Non pubblicare immagini, distribuire servizi su Internet o eseguire push
  senza una richiesta esplicita.
- Conservare modifiche preesistenti dell'utente e licenze del progetto.
- Comunicare in italiano e descrivere chiaramente risultati e limiti.

## Architettura prevista

- Il container serve i file statici; il gioco viene eseguito nel browser
  dell'utente tramite JavaScript e WebAssembly.
- Usare come primo percorso i file già inclusi: `index.html`, `zarrosim.js`,
  `zarrosim.wasm` e `resources/`. Verificarne completezza e funzionamento.
- Servire il gioco su un indirizzo stabile, preferibilmente
  `http://localhost:8080`, pubblicando la porta soltanto su `127.0.0.1`.
- Non richiedere Emscripten, Python o Node sul computer dell'utente per avviare
  il gioco. Docker con Compose è il requisito previsto.
- Usare un server statico leggero e mantenuto, con MIME corretto per `.wasm`.
  Evitare dipendenze e servizi non necessari.
- Non presumere che il browser possa aprire direttamente `index.html` via
  `file://`: l'interfaccia usa richieste per caricare le risorse.
- Se i file compilati non funzionano o richiedono modifiche, predisporre una
  compilazione riproducibile in Docker, preferibilmente multistadio.
  Fissare versioni e revisione della dipendenza Novantotto; il Makefile
  attuale la scarica senza fissarne la revisione.

## Attività richieste

1. Esaminare caricamento delle risorse, dipendenze, audio e salvataggi prima
   di scegliere i dettagli della configurazione.
2. Aggiungere `Dockerfile`, `.dockerignore` e un file Compose. Includere nel
   container soltanto i file necessari; escludere metadati Git, credenziali,
   directory personali e materiali di sviluppo non utilizzati.
3. Rendere l'avvio possibile con `docker compose up --build -d` e documentare
   indirizzo, arresto, aggiornamento e risoluzione dei problemi comuni.
4. Rendere il gioco utilizzabile senza connessione Internet dopo la build
   dell'immagine. Identificare le risorse remote ed eliminare o rendere locali
   quelle necessarie; conservare attribuzioni e licenze.
5. Esaminare l'effettiva implementazione dei salvataggi, anche nella dipendenza
   Novantotto se necessario. Non presumere che un volume Docker salvi lo
   stato del browser. Garantire persistenza tra chiusura e riapertura e
   documentare dove risiedono i dati e come evitarne la perdita. Se necessario,
   introdurre una soluzione minima per la persistenza o esportazione e
   importazione. Non dichiarare compatibilità con i vecchi salvataggi Windows
   senza averla verificata.
6. Conservare l'aspetto Windows 95/98 e la logica di gioco; evitare modifiche
   estetiche o funzionalità estranee all'obiettivo.

## Verifica e criteri di completamento

- Costruire e avviare realmente il container, nel rispetto delle autorizzazioni
  dell'ambiente. Verificare la validità della configurazione Compose.
- Verificare HTTP, caricamento WebAssembly, MIME e assenza di risorse mancanti.
- Quando è disponibile un browser controllabile, verificare avvio del gioco,
  alcune finestre e azioni rappresentative, audio dopo interazione dell'utente
  e salvataggio dopo chiusura e riapertura. Verificare anche il salvataggio dopo
  riavvio o ricreazione del container.
- Verificare che le risorse necessarie non richiedano servizi esterni durante
  il gioco e che la porta sia esposta soltanto localmente.
- Non introdurre suite di test complesse per la sola configurazione: preferire
  verifiche concrete proporzionate al cambiamento.
- Se una verifica è impossibile, indicare esattamente cosa non è stato
  verificato e perché; non presentare controlli HTTP come prova del gameplay.
- Aggiornare il README con istruzioni Linux/Docker concise e riproducibili,
  comportamento dei salvataggi e limitazioni effettivamente riscontrate.
- Concludere indicando file modificati, comando di avvio, URL e verifiche
  eseguite. Non creare commit o push salvo richiesta dell'utente.
