# Novantotto vendorizzato

Sorgente: https://github.com/andreax79/novantotto
Revisione: `12176b4d630bfb2d7426ff8b5ddf81bbc875e550` (3 luglio 2024).
Licenza originale MPL-2.0 in `LICENSE` e nelle intestazioni dei file.

La copia locale evita download di revisioni variabili durante la compilazione.
Il Makefile e la build Docker usano questa directory.

Modifiche locali in `handle.c`:

- Il rilascio di una finestra identifica i figli tramite l'handle numerico
  della finestra, anziché confrontarlo con il puntatore alla struttura.
- I figli vengono rilasciati ricorsivamente, liberando nomi di classe e proprietà.
- Le strutture liberate vengono azzerate prima del riuso, evitando puntatori
  e callback appartenenti ai controlli precedenti.
- Un fallimento nell'allocazione delle proprietà non libera una singola
  struttura che appartiene all'array della tabella.

In `dialog.c`, `DialogBox` conserva il valore restituito prima di distruggere
la finestra e azzerare lo slot, anziché leggerlo dopo il rilascio.

Regressione verificata da `tests/novantotto-handles.c` e
`tests/novantotto-dialog-result.c` nella radice del progetto: 500 cicli di
dialoghi annidati con controlli e riutilizzo degli slot, e 50 chiusure con
verifica del risultato restituito al chiamante.
