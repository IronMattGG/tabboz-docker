novantotto_home = vendor/novantotto
NODE ?= node
sources = disco.c \
		  eventi.c \
		  lavoro.c \
		  proteggi.c \
		  readkey.c \
		  scooter.c \
		  scuola.c \
		  tabimg.c \
		  telefono.c \
		  tempo.c \
		  tipa.c \
		  vestiti.c \
		  zarrosim.c \
		  $(novantotto_home)/*.c

.PHONY: build
build: copy
	emcc $(sources) -o zarrosim.js \
		$(CFLAGS) \
		-I $(novantotto_home) \
		-s ASYNCIFY \
		-s GLOBAL_BASE=66536 \
		-s ASSERTIONS=2 \
		-s SAFE_HEAP=1 \
		-s STACK_OVERFLOW_CHECK=2 \
		-s DEMANGLE_SUPPORT=1 \
		-s 'ASYNCIFY_IMPORTS=["emscripten_asm_const_int"]' \
		-s 'EXPORTED_RUNTIME_METHODS=["ccall", "setValue"]' \
		-s 'EXPORTED_FUNCTIONS=["_main", "_GetDlgItem", "_PostMessage", "_PrintMessages", "_AllocateControl", "_GetTickCount", "_WinMainStartup"]'

.PHONY: format
format:
	clang-format -i *.h *.c

.PHONY: test
test:
	emcc tests/novantotto-handles.c $(novantotto_home)/handle.c $(novantotto_home)/property.c \
		-I $(novantotto_home) -s ASSERTIONS=2 -s EXIT_RUNTIME=1 -s ENVIRONMENT=node \
		-o /tmp/novantotto-handles.js
	$(NODE) /tmp/novantotto-handles.js
	emcc tests/novantotto-dialog-result.c $(novantotto_home)/dialog.c $(novantotto_home)/handle.c $(novantotto_home)/property.c \
		-I $(novantotto_home) -s ASSERTIONS=2 -s EXIT_RUNTIME=1 -s ENVIRONMENT=node \
		-s ASYNCIFY -s 'ASYNCIFY_IMPORTS=["emscripten_asm_const_int"]' \
		-o /tmp/novantotto-dialog-result.js
	$(NODE) /tmp/novantotto-dialog-result.js

.PHONY: export
export: copy
	RESOURCES_DIR="./resources" $(novantotto_home)/utils/export.sh *.RES

.PHONY: rc
rc: copy
	RESOURCES_DIR="./resources/" $(novantotto_home)/utils/rc.sh resources/dialogs/dialogs.rc
	RESOURCES_DIR="./resources/" $(novantotto_home)/utils/index_gen.sh

.PHONY: copy
copy: $(novantotto_home)
	@mkdir -p resources/bitmaps/novantotto
	@cp -r $(novantotto_home)/bitmaps/* resources/bitmaps/novantotto
	@mkdir -p resources/icons/novantotto
	@cp -r $(novantotto_home)/icons/* resources/icons/novantotto
	@mkdir -p resources/css
	@cp -r $(novantotto_home)/css/* resources/css
	@mkdir -p resources/js
	@cp -r $(novantotto_home)/js/* resources/js
